#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include <sys/prctl.h>

#include <linux/videodev2.h>
#include <linux/media.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

int main(int argc, char **argv){
    
    int fd;
    if( argc > 1){
        fd = open(argv[1], O_RDWR);
    }else{
        fd = open("/dev/video0", O_RDWR, 0);
    }
    

    if( fd == -1){
        printf("%s\n", strerror(errno));
        return 1;
    }

    //I/O, functions supported and general info
    struct v4l2_capability capability;
    if( ioctl( fd, VIDIOC_QUERYCAP, &capability ) ){
    	printf("%s\n", strerror(errno));
	    return 2;
    }
    printf("Driver: %s\tDevice: %s\nDriver version: %d\tBus info: %s\n", capability.driver, capability.card, capability.version, capability.bus_info);
    printf("Capability flag: %d\n\n", capability.device_caps);

    //
    struct v4l2_input input;
    int input_index;
    if( ioctl(fd, VIDIOC_G_INPUT, &input_index) == -1 ){
        printf("%s\n", strerror(errno));
        return 4;
    }
    
    memset(&input, 0, sizeof(input));
    input.index = input_index;
    
    if( ioctl(fd, VIDIOC_ENUMINPUT, &input) == -1 ){
        printf("%s\n", strerror(errno));
        return 5;
    }
    printf("Index: %d\tName: %s\nTuner: %d\tInput type: %d\nStatus: %lu\tCapabilities: %d\n", input.index, input.name, input.tuner, input.type, (long unsigned int)input.status, input.capabilities);
    putchar('\n');
    //

    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    //set format
    struct v4l2_format format;
    format.type = type;
    format.fmt.pix.height = 480;
    format.fmt.pix.width = 640;
    format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
    format.fmt.pix.field = V4L2_FIELD_INTERLACED;
    if( ioctl( fd, VIDIOC_S_FMT, &format ) == -1 ){
        printf("%s %d\n", strerror(errno), errno);
        return 6;
    }
    printf("Format setted\n");

    int streaming_method = V4L2_MEMORY_USERPTR;

    //initializate I/O method
    struct v4l2_requestbuffers buff;
    buff.count = 1;
    buff.memory = streaming_method;
    buff.type = type;
    if( ioctl( fd, VIDIOC_REQBUFS, &buff ) == -1 ){
        printf("%s %d\n", strerror(errno), errno);
        return 7;
    }
    printf("I/O method setted\tNumber of buffers: %d\nCapabilities: %d\n", buff.count, buff.capabilities);

    //buff
    uint8_t my_buff[format.fmt.pix.sizeimage];
    struct v4l2_buffer buffer;
    memset( &buffer, 0, sizeof(struct v4l2_buffer) );
    buffer.index = 0;
    buffer.type = type;
    buffer.memory = streaming_method;
    buffer.flags = 0;
    buffer.m.userptr = (long unsigned int)my_buff;
    buffer.length = format.fmt.pix.sizeimage;
    if( ioctl( fd, VIDIOC_QBUF, &buffer ) == -1 ){
        printf("%s %d\n", strerror(errno), errno);
        return 8;
    }

    //starting capture
    if( ioctl( fd, VIDIOC_STREAMON, &type ) == -1 ){
        printf("%s %d\n", strerror(errno), errno);
        return 9;
    }
    printf("Capture started\n\n");

    sleep(1);

    //stopping capture
    if( ioctl( fd, VIDIOC_STREAMOFF, &type ) == -1 ){
        printf("%s %d\n", strerror(errno), errno);
        return 10;
    }
    printf("Capture stopped\n");

    close(fd);

    return 0;
}
