#include <stdio.h>

#include <unistd.h>

#include <fcntl.h>

#include <errno.h>

#include <stdlib.h>

#include <string.h>

#include <termios.h>

#include <unistd.h>

int set_interface_attribs(int fd, int speed)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

int main() {

    char byte[1];

    int fd = -1;

    fd = open("/dev/ttyPS0", O_RDWR);
    set_interface_attribs(fd, B115200);
    if (fd == -1){
	printf("Error\n");
    } else {
	printf("Connected\n");
    }
   
    printf("open sensor port 2\n");
    write(fd,"on 2\n",5);

    tcflush(fd,TCIOFLUSH);

    if(write(fd, "loserville\n", 11)!=11){
	printf("Error writing\n");
    }
    printf("Written\n");
    usleep(500000);
    FILE *fp;
    fp = fopen("/var/ftp/log.csv","w");
    int x = 0;
    while(x < 17560){
	read(fd,byte,1);
	printf("%c",byte[0]);
	fprintf(fp,"%c",byte[0]);
	x++;
    }
    write(fd,"off 2\n",7);
    tcflush(fd,TCIOFLUSH);
    //write(fd,"off 2\n",6);
    close(fd);
    fclose(fp);

    return 0;

}
