#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/freetype.hpp>

#include <map>
#include <vector>
#include <string>
#include <iostream>

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
          ft2->putText(output_image, "无法识别", cv::Point(x1, y1+15), 15, CV_RGB(255, 0, 0), cv::FILLED, cv::LINE_AA, true);
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

int main(int argc, char** argv) {

    // Instantiate YuNet
    // YuNet model(model_path, cv::Size(320, 320), 0.9, 0.3, 500, backend_id, target_id);


    ft2 = cv::freetype::createFreeType2();
    ft2->loadFontData("SimHei.ttf", 0);

    YuNet model("face_detection_yunet_2023mar.onnx");
    Ptr<FaceRecognizerSF> faceRecognizer = FaceRecognizerSF::create("face_recognition_sface_2021dec.onnx", "");

    double cosine_similar_thresh = 0.363;
    double l2norm_similar_thresh = 1.128;

    int device_id = 0;
    auto cap = cv::VideoCapture(device_id);

    int w = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int h = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    model.setInputSize(cv::Size(w, h));

    /* 导入人脸图片 */
    String input1 = "./lisibo.jpg";
    cv::Mat image1 = imread(samples::findFile(input1));
    if (image1.empty())
    {
        std::cerr << "Cannot read image: " << input1 << std::endl;
        return 2;
    }
    resize(image1, image1, cv::Size(w, h));
    cv::Mat faces1 = model.infer(image1);
    if (faces1.rows < 1)
       std::cerr << "Cannot find a face in image1 " << std::endl; 

    Mat aligned_face1;
    faceRecognizer->alignCrop(image1, faces1.row(0), aligned_face1);
    Mat feature1;
    faceRecognizer->feature(aligned_face1, feature1);
    feature1 = feature1.clone();

    auto tick_meter = cv::TickMeter();
    cv::Mat frame;
    while (cv::waitKey(1) < 0)
    {
        bool has_frame = cap.read(frame);
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
        } else {
          Mat feature2, aligned_face2;
          faceRecognizer->alignCrop(frame, faces.row(0), aligned_face2);
          faceRecognizer->feature(aligned_face2, feature2);
          feature2 = feature2.clone();

          //! [match]
          double cos_score = faceRecognizer->match(feature1, feature2, FaceRecognizerSF::DisType::FR_COSINE);
          double L2_score = faceRecognizer->match(feature1, feature2, FaceRecognizerSF::DisType::FR_NORM_L2);

          if (cos_score >= cosine_similar_thresh && (L2_score <= l2norm_similar_thresh) )
          {
              std::cout << "cos_score = " << cos_score << ", L2_score = " << L2_score;
              std::cout << "They have the same identity;" << std::endl;
              name = "小李子";
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

