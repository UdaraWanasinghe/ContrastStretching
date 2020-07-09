#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

struct Histogram {
    int columns[256] = {0};
    int width = 512;
    int height = 256;
};

void transform_hist(Histogram *histogram, int *arr, int p = 5);

void get_histogram(Mat image, Histogram *histogram);

void show_histogram(string window, Histogram *histogram);

int main(int argc, char **argv) {
    if (argc < 2) {
        cout << "Image path is required" << endl;
        return 128;
    }
    string path = argv[1];
    Mat image = imread(path, IMREAD_GRAYSCALE);
    if (image.empty()) {
        cout << "Invalid image: " << argv[1] << endl;
        return 128;
    }
    imshow("Input_Image", image);

    struct Histogram histogram1;
    get_histogram(image, &histogram1);
    show_histogram("Input_Histogram", &histogram1);

    int trans_arr[256];
    if (argc < 3) {
        transform_hist(&histogram1, trans_arr);
    } else {
        int p = stoi(argv[2]);
        transform_hist(&histogram1, trans_arr, p);
    }
    Mat outputImage = image.clone();
    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            int v = (int) image.at<uchar>(i, j);
            outputImage.at<uchar>(i, j) = trans_arr[v];
        }
    }
    imshow("Output_Image", outputImage);
    Histogram histogram2;
    get_histogram(outputImage, &histogram2);
    show_histogram("Output_Histogram", &histogram2);
    waitKey(0);
    image.release();
    outputImage.release();
    return 0;
}

void transform_hist(Histogram *histogram, int *arr, int p) {
    int l;
    for (int i = 0; i < 256; i++) {
        if (histogram->columns[i] > 0) {
            l = i;
            break;
        }
    }
    int u;
    for (int i = 255; i >= 0; i--) {
        if (histogram->columns[i] > 0) {
            u = i;
            break;
        }
    }
    int a = l + (u - l) * p / 100;
    int b = u - (u - l) * p / 100;
    int c = 255 * p / 100;
    int d = 255 * (100 - p) / 100;
    printf("a: %d\n", a);
    printf("b: %d\n", b);
    printf("c: %d\n", c);
    printf("d: %d\n", d);
    for (int i = 0; i < a; i++) {
        arr[i] = i * c / a;
    }
    for (int i = a; i < b; i++) {
        arr[i] = c + (i - a) * (d - c) / (b - a);
    }
    for (int i = b; i < 256; i++) {
        arr[i] = d + (i - b) * (255 - d) / (255 - b);
    }
}

void get_histogram(Mat image, Histogram *histogram) {
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.rows; j++) {
            int v = (int) image.at<uchar>(i, j);
            histogram->columns[v]++;
        }
    }
}

void show_histogram(string window, Histogram *histogram) {
    Mat image(histogram->height, histogram->width, CV_8UC1, Scalar(255));
    int thickness = histogram->width / 256;
    int max;
    for (int i = 0; i < 256; i++) {
        if (max < histogram->columns[i]) {
            max = histogram->columns[i];
        }
    }
    for (int i = 0; i < 256; i++) {
        int colH = histogram->columns[i] * histogram->height / max;
        Point start = Point(i * thickness, histogram->height);
        Point end = Point(i * thickness, histogram->height - colH);
        line(image,
             start,
             end,
             Scalar(0),
             1);
    }
    imshow(window, image);
}