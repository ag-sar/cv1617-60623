#include <pcl/io/openni_grabber.h>
#include <pcl/visualization/cloud_viewer.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>

using namespace pcl;

char key = '1'; 

class SimpleOpenNIViewer {
    public:
        visualization::CloudViewer* viewer; 
     
    static void pcl_keyboard_callback(const visualization::KeyboardEvent& event, void* viewer_void) {
        if(event.keyDown()) {
            char keyCode = event.getKeyCode();
            switch(keyCode) {
                case 87:  // W
                case 119: // w
                    key ='w';
                    break;
            }
        }
    }
      
    SimpleOpenNIViewer() { 
        viewer = new visualization::CloudViewer("PCL OpenNI Viewer"); 
        viewer -> registerKeyboardCallback(SimpleOpenNIViewer::pcl_keyboard_callback, (void*)NULL); 
    } 

    void cloud_cb_ (const PointCloud<PointXYZRGB>::ConstPtr &cloud) {
        if (!viewer -> wasStopped()) {
            viewer -> showCloud (cloud);
            if (key == 'w') {
                io::savePCDFileASCII ("test_1.pcd", *cloud);
                key = '1';
            }
        }
    }

    void run() {
        Grabber* interface = new OpenNIGrabber();

        boost::function<void (const PointCloud<PointXYZRGB>::ConstPtr&)> f =
            boost::bind (&SimpleOpenNIViewer::cloud_cb_, this, _1);

        interface -> registerCallback(f);

        interface -> start();
       
        while (!viewer -> wasStopped()) {
            sleep(1);
            //boost::this_thread::sleep (boost::posix_time::seconds (1));
        }

        interface -> stop();
    }
};

int main() {
    SimpleOpenNIViewer v;
    v.run();
    
    return 0;
}