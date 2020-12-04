#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <sys/prctl.h>

#include <linux/videodev2.h>
#include <sys/ioctl.h>

int main(int argc, char **argv){
    int fd = open("/dev/video0", O_RDWR);

    if( fd == -1){
        printf("%s\n", strerror(errno));
        return 1;
    }

    struct v4l2_input input;
    int index;
    if( ioctl(fd, VIDIOC_G_INPUT, &index) == -1 ){
        printf("%s\n", strerror(errno));
        return 2;
    }
    
    memset(&input, 0, sizeof(input));
    input.index = index;
    
    if( ioctl(fd, VIDIOC_ENUMINPUT, &input) == -1 ){
        printf("%s\n", strerror(errno));
        return 3;
    }

    printf("%s, %d\n", input.name, input.type);

    //I/O buffer
    struct v4l2_requestbuffers req_buf;
    memset(&req_buf, 0, sizeof(req_buf));
    req_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req_buf.memory = V4L2_MEMORY_MMAP;
    req_buf.count = 20;

    if( ioctl(fd, VIDIOC_REQBUFS, &req_buf) == -1 ){
        printf("%s\n", strerror(errno));
        return 3;
    }
    printf("%d\n", req_buf.memory);

    close(fd);

    return 0;
}