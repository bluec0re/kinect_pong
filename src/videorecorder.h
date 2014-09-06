#ifndef VIDEORECORDER_H
#define VIDEORECORDER_H

#include <opencv2/opencv.hpp>
#include <Ogre.h>

class OgreCvRecorder
{
protected:
    Ogre::String mFileName;
    CvVideoWriter *writer;
    bool record;
public:
   OgreCvRecorder(const Ogre::String& filename)
     : writer(0), record(false), mFileName(filename)
   {
   }

   void start(){ record = true; }
   void stop(){ record = false; }

   void update(Ogre::RenderWindow* win)
   {
        CvSize size = cvSize (win->getWidth(), win->getHeight());
/*
        Other possible codec codes:
        CV_FOURCC('P','I','M','1')    = MPEG-1 codec
        CV_FOURCC('M','J','P','G')    = motion-jpeg codec (does not work well)
        CV_FOURCC('M', 'P', '4', '2') = MPEG-4.2 codec
        CV_FOURCC('D', 'I', 'V', '3') = MPEG-4.3 codec
        CV_FOURCC('D', 'I', 'V', 'X') = MPEG-4 codec
        CV_FOURCC('U', '2', '6', '3') = H263 codec
        CV_FOURCC('I', '2', '6', '3') = H263I codec
        CV_FOURCC('F', 'L', 'V', '1') = FLV1 codec
*/
       if (!writer) {
           writer = cvCreateVideoWriter(
                mFileName.c_str(),
                CV_FOURCC('P','I','M','1'),
                30, // set fps
                size  );
        }

        if (record) {
            float width  = win->getWidth();
            float height = win->getHeight();

            IplImage *mCvCaptureImage, *mCvConvertImage;
            mCvCaptureImage = cvCreateImage( cvSize( width,height ), IPL_DEPTH_8U, 3);
            mCvConvertImage = cvCreateImage( cvSize( width,height ), IPL_DEPTH_8U, 3);
            Ogre::PixelFormat pf = win->suggestPixelFormat();
            Ogre::PixelBox pb(width, height, 1, pf, mCvCaptureImage->imageData);
            win->copyContentsToMemory(pb);
            // swap red and blue channels
            cvConvertImage(mCvCaptureImage, mCvConvertImage, CV_CVTIMG_SWAP_RB);
            cvWriteFrame( writer, mCvConvertImage );
            cvReleaseImage(&mCvConvertImage);
            cvReleaseImage(&mCvCaptureImage);
        }
    }

   ~OgreCvRecorder()
   {
       cvReleaseVideoWriter( &writer );
   }

};

#endif // VIDEORECORDER_H
