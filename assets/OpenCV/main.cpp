#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/freetype.hpp>

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>

#include <gtk/gtk.h>


using namespace cv;
using namespace std;


class YuNet
{
public:
    YuNet(const std::string& model_path,
          const cv::Size& input_size = cv::Size(320, 320),
          float conf_threshold = 0.6f,
          float nms_threshold = 0.3f,
          int top_k = 5000,
          int backend_id = 0,
          int target_id = 0)
        : model_path_(model_path), input_size_(input_size),
          conf_threshold_(conf_threshold), nms_threshold_(nms_threshold),
          top_k_(top_k), backend_id_(backend_id), target_id_(target_id)
    {
        model = cv::FaceDetectorYN::create(model_path_, "", input_size_, conf_threshold_, nms_threshold_, top_k_, backend_id_, target_id_);
    }

    /* Overwrite the input size when creating the model. Size format: [Width, Height].
    */
    void setInputSize(const cv::Size& input_size)
    {
        input_size_ = input_size;
        model->setInputSize(input_size_);
    }

    cv::Mat infer(const cv::Mat image)
    {
        cv::Mat res;
        model->detect(image, res);
        return res;
    }

private:
    cv::Ptr<cv::FaceDetectorYN> model;

    std::string model_path_;
    cv::Size input_size_;
    float conf_threshold_;
    float nms_threshold_;
    int top_k_;
    int backend_id_;
    int target_id_;
};

/* 中文字体 */
cv::Ptr<cv::freetype::FreeType2> ft2;

cv::Mat visualize(const cv::Mat& image, const cv::Mat& faces, float fps, const char* name)
{
    static cv::Scalar box_color{0, 255, 0};
    static std::vector<cv::Scalar> landmark_color{
        cv::Scalar(255,   0,   0), // right eye
        cv::Scalar(  0,   0, 255), // left eye
        cv::Scalar(  0, 255,   0), // nose tip
        cv::Scalar(255,   0, 255), // right mouth corner
        cv::Scalar(  0, 255, 255)  // left mouth corner
    };
    static cv::Scalar text_color{255, 0, 0};

    auto output_image = image.clone();

    if (fps >= 0)
    {
        cv::putText(output_image, cv::format("FPS: %.2f", fps), cv::Point(0, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, text_color, 2);
    }

    for (int i = 0; i < faces.rows; ++i)
    {
        // Draw bounding boxes
        int x1 = static_cast<int>(faces.at<float>(i, 0));
        int y1 = static_cast<int>(faces.at<float>(i, 1));
        int w = static_cast<int>(faces.at<float>(i, 2));
        int h = static_cast<int>(faces.at<float>(i, 3));


        if (name != NULL) { /* 识别成功 */
          cv::rectangle(output_image, cv::Rect(x1, y1, w, h), CV_RGB(0, 255, 0), 2);
          ft2->putText(output_image, cv::format("%s", name), cv::Point(x1, y1+30), 30, CV_RGB(0, 255, 0), cv::FILLED, cv::LINE_AA, true);
        } else { /* 识别失败 */
          cv::rectangle(output_image, cv::Rect(x1, y1, w, h), CV_RGB(255, 0, 0), 2);
          if (faces.rows == 1)
            ft2->putText(output_image, "请录入人脸", cv::Point(x1, y1+15), 15, CV_RGB(255, 0, 0), cv::FILLED, cv::LINE_AA, true);
          else 
            ft2->putText(output_image, "人脸过多，请重试", cv::Point(x1, y1+15), 15, CV_RGB(255, 0, 0), cv::FILLED, cv::LINE_AA, true);
        }
        

        // Confidence as text
        // float conf = faces.at<float>(i, 14);
        // cv::putText(output_image, cv::format("%.4f", conf), cv::Point(x1, y1+12), cv::FONT_HERSHEY_DUPLEX, 0.5, text_color);
        

        // Draw landmarks
        // for (int j = 0; j < landmark_color.size(); ++j)
        // {
        //     int x = static_cast<int>(faces.at<float>(i, 2*j+4)), y = static_cast<int>(faces.at<float>(i, 2*j+5));
        //     cv::circle(output_image, cv::Point(x, y), 2, landmark_color[j], 2);
        // }
    }
    return output_image;
}

YuNet model("face_detection_yunet_2023mar.onnx");
cv::Ptr<cv::FaceRecognizerSF> faceRecognizer;
int w, h;

static guint employee_count = 0;
static char employee_name[100][100];
Mat face_feature[100];


static void 
read_face_data () {

  /* 找到图片路径 */
  DIR *dir;
  struct dirent *entry;
  const char *path = "/home/lieryang/Desktop/LieryangStack.github.io/assets/OpenCV";

  if ((dir = opendir(path)) == NULL) {
    perror("opendir");
    return;
  }

  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_REG) {
      char file_path[100];
      const char *filename = entry->d_name;
      const char *ext = strrchr(filename, '.');
      if (ext != NULL && strcmp(ext, ".jpg") == 0) {  
        g_snprintf (employee_name[employee_count], strlen(filename) - 3, "%s", filename);
        g_snprintf (file_path, 100, "%s/%s", path, filename);

        cv::Mat image = cv::imread(samples::findFile(file_path));
        if (image.empty())
          continue;
        
        cv::resize(image, image, cv::Size(w, h));
        cv::Mat faces = model.infer(image);
        if (faces.rows < 1)
            std::cerr << "Cannot find a face in image1 " << std::endl; 

        Mat aligned_face;
        faceRecognizer->alignCrop(image, faces.row(0), aligned_face);
        faceRecognizer->feature(aligned_face, face_feature[employee_count]);
        face_feature[employee_count] = face_feature[employee_count].clone();

        // printf("%s/%s %s\n", path, filename, filename);
        // printf ("name = %s\n", employee_name[employee_count]);

        employee_count++;

        if (employee_count > 99)
          break;
      } 
    }
  }

  closedir(dir);
}

int main(int argc, char** argv) {

  // Instantiate YuNet
  // YuNet model(model_path, cv::Size(320, 320), 0.9, 0.3, 500, backend_id, target_id);

  ft2 = cv::freetype::createFreeType2();
  ft2->loadFontData("SimHei.ttf", 0);

  faceRecognizer = cv::FaceRecognizerSF::create("face_recognition_sface_2021dec.onnx", "");

  double cosine_similar_thresh = 0.363;
  double l2norm_similar_thresh = 1.128;

  auto cap = cv::VideoCapture(0);

  w = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
  h = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));

  if (w != 640) w = 640;
  if (h != 480) h = 480;
  model.setInputSize(cv::Size(w, h));

  read_face_data();

  auto tick_meter = cv::TickMeter();

  while (cv::waitKey(1) < 0)
  {
    cv::Mat raw_frame;
    cv::Mat frame;
    bool has_frame = cap.read(raw_frame);
    cv::resize(raw_frame, frame, cv::Size(640, 480));
    if (!has_frame)
    {
        std::cout << "No frames grabbed! Exiting ...\n";
        break;
    }

    const char *name = NULL;

    // Inference
    tick_meter.start();
    cv::Mat faces = model.infer(frame);
    static long int count = 0;
    if (faces.rows < 1) {
      std::cerr << "Cannot find a face in Frame : " << count++ << std::endl; 
    } else if (faces.rows == 1) {
      Mat feature2, aligned_face2;
      faceRecognizer->alignCrop(frame, faces.row(0), aligned_face2);
      faceRecognizer->feature(aligned_face2, feature2);
      feature2 = feature2.clone();

      for (int i = 0; i < employee_count; i++) {

        //! [match]
        double cos_score = faceRecognizer->match(face_feature[i], feature2, FaceRecognizerSF::DisType::FR_COSINE);
        double L2_score = faceRecognizer->match(face_feature[i], feature2, FaceRecognizerSF::DisType::FR_NORM_L2);

        if (cos_score >= cosine_similar_thresh && (L2_score <= l2norm_similar_thresh) ) {
            // std::cout << "cos_score = " << cos_score << ", L2_score = " << L2_score;
            // std::cout << "They have the same identity;" << std::endl;
            name = employee_name[i];
        }

      }          
    }
    tick_meter.stop();

    // Draw results on the input image
    auto res_image = visualize(frame, faces, (float)tick_meter.getFPS(), name);
    // Visualize in a new window
    cv::imshow("YuNet Demo", res_image);

    tick_meter.reset();
  }

  return 0;
}

