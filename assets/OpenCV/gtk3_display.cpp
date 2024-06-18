/**
 * /usr/bin/g++ -g gtk3_display.cpp -o gtk3_display `pkg-config --cflags --libs gtk+-3.0 \
   /usr/local/opencv-4.9.0/lib/pkgconfig/opencv4.pc` -Wl,-rpath,/usr/local/opencv-4.9.0/lib
 */

#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/freetype.hpp>

#include <map>
#include <vector>
#include <string>
#include <iostream>
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

cv::Mat visualize(const cv::Mat& image, const cv::Mat& faces)
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

    for (int i = 0; i < faces.rows; ++i)
    {
        // Draw bounding boxes
        int x1 = static_cast<int>(faces.at<float>(i, 0));
        int y1 = static_cast<int>(faces.at<float>(i, 1));
        int w = static_cast<int>(faces.at<float>(i, 2));
        int h = static_cast<int>(faces.at<float>(i, 3));

        cv::rectangle(output_image, cv::Rect(x1, y1, w, h), CV_RGB(0, 255, 0), 2);

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



GtkWidget *window;
YuNet model("face_detection_yunet_2023mar.onnx");
cv::Mat frame;
VideoCapture cap;
int w, h;

static int
update_img (gpointer data) {
  GtkImage *img_widget = GTK_IMAGE (data);

  bool has_frame = cap.read(frame);
  cv::resize(frame, frame, cv::Size(640, 480));

  cv::Mat faces = model.infer(frame);

  cv::Mat res_image = visualize(frame, faces);

  cv::cvtColor(res_image, res_image, cv::COLOR_BGR2RGB);
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data (res_image.data, GDK_COLORSPACE_RGB, false, 8, w, h, res_image.step, NULL, NULL);

  gtk_image_set_from_pixbuf (img_widget, pixbuf);

  return TRUE;
}


/**
 * @brief: 获取图像
 */
static void 
capture_button_clicked (GtkWidget *widget, gpointer data) {
  GtkImage *img_widget = GTK_IMAGE (data);

  bool has_frame = cap.read(frame);
  cv::resize(frame, frame, cv::Size(640, 480));

  cv::Mat faces = model.infer(frame);

  cv::Mat res_image = visualize(frame, faces);

  cv::cvtColor(res_image, res_image, cv::COLOR_BGR2RGB);
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data (res_image.data, GDK_COLORSPACE_RGB, false, 8, w, h, res_image.step, NULL, NULL);

  gtk_image_set_from_pixbuf (img_widget, pixbuf);
}


/**
 * @brief: 保存图像
 */
static void
write_button_clicked (GtkWidget *widget, gpointer data) {

  GtkWidget *dialog;
  GtkEntry *entry = GTK_ENTRY (data);

  const char *name = gtk_entry_get_text (entry);

  cv::Mat faces = model.infer(frame);

  if (name && faces.rows == 1) {
    char file_name[100];
    g_snprintf (file_name, 100, "%s.jpg", name);
    imwrite (file_name, frame);

    // 创建一个消息对话框
    dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                    GTK_DIALOG_MODAL,
                                    GTK_MESSAGE_INFO,
                                    GTK_BUTTONS_OK,
                                    "信息录入成功!");
  
  } else {
    dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                    GTK_DIALOG_MODAL,
                                    GTK_MESSAGE_INFO,
                                    GTK_BUTTONS_OK,
                                    "信息录入失败，请调整姿势，保持一个人脸!");
  }

  // 运行对话框并等待用户响应
  gtk_dialog_run(GTK_DIALOG(dialog));

  // 关闭对话框
  gtk_widget_destroy(dialog);


}

int main(int argc, char *argv[]) {
  /* 设置OpenCV */
  ft2 = cv::freetype::createFreeType2();
  ft2->loadFontData("SimHei.ttf", 0);

  cv::Ptr<cv::FaceRecognizerSF> faceRecognizer = cv::FaceRecognizerSF::create("face_recognition_sface_2021dec.onnx", "");

  double cosine_similar_thresh = 0.363;
  double l2norm_similar_thresh = 1.128;

  int device_id = 0;
  cap = cv::VideoCapture(device_id);

  w = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
  h = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
  if (w != 640) w = 640;
  if (h != 480) h = 480;
  model.setInputSize(cv::Size(w, h));

  bool has_frame = cap.read(frame);

  cv::resize(frame, frame, cv::Size(640, 480));

  cv::Mat faces = model.infer(frame);

  // Draw results on the input image
  cv::Mat res_image = visualize(frame, faces);

  // 初始化GTK
  gtk_init(&argc, &argv);

  // 创建主窗口
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "Vertical Box Example");
  gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  // 创建垂直box
  GtkWidget *main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_container_add(GTK_CONTAINER(window), main_vbox);

  // 创建图像控件
  // std::cout << "Color space format: " << res_image.type() << std::endl;
  cv::cvtColor(res_image, res_image, cv::COLOR_BGR2RGB);
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data (res_image.data, GDK_COLORSPACE_RGB, false, 8, w, h, res_image.step, NULL, NULL);
  // GtkWidget *image = gtk_image_new_from_file("lisibo.jpg");
  GtkWidget *image = gtk_image_new_from_pixbuf (pixbuf);
  gtk_box_pack_start(GTK_BOX(main_vbox), image, TRUE, TRUE, 0);

  // 创建Entry
  GtkWidget *entry = gtk_entry_new();
  gtk_entry_set_text(GTK_ENTRY(entry), "请输入员工姓名");
  gtk_box_pack_start(GTK_BOX(main_vbox), entry, FALSE, FALSE, 0);

  // 创建垂直box
  GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(main_vbox), hbox, FALSE, FALSE, 0);

  // 创建Button
  GtkWidget *capture_button = gtk_button_new_with_label("获取图像");
  g_signal_connect(capture_button, "clicked", G_CALLBACK(capture_button_clicked), image);
  gtk_box_pack_start(GTK_BOX(hbox), capture_button, TRUE, TRUE, 0);

  // 创建Button
  GtkWidget *write_button = gtk_button_new_with_label("录入人脸");
  g_signal_connect(write_button, "clicked", G_CALLBACK(write_button_clicked), entry);
  gtk_box_pack_start(GTK_BOX(hbox), write_button, TRUE, TRUE, 0);

  // 显示窗口和内容
  gtk_widget_show_all(window);

  g_idle_add (update_img, image);

  // 进入GTK主循环
  gtk_main();

  return 0;
}

