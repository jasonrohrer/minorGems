#include "SoundPlayer.h"
#include <stdio.h>
#include </usr/include/sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include </usr/include/sys/soundcard.h>
#include <stdio.h>
#include <iostream.h>

/** set up /dev/dsp*/
void SoundPlayer::setUpAudio(){
  
  if((audio_fd = open("/dev/dsp", O_WRONLY, 0))==-1){
    perror("/dev/dsp");
    exit(1);
  }
  
  int format = AFMT_S16_BE;
  if(ioctl(audio_fd, SNDCTL_DSP_SETFMT, &format) == -1){
    perror("SNDCTL_DSP_SETFMT");

  }

  int speed = 44100;
  if(ioctl(audio_fd, SNDCTL_DSP_SPEED, &speed) == -1){
    perror("SNDCTRL_DSP_SPPED");

  }
  int channels = 1;
  if(ioctl(audio_fd, SNDCTL_DSP_CHANNELS, &channels)==-1){
    perror("SNDCTL_DSP_CHANNELS");
  }
  
  //try to set the frag size
  int frag = 0x000a000a;
  if (ioctl(audio_fd, SNDCTL_DSP_SETFRAGMENT, &frag)==-1){
    perror("Problem setting the fragment size");
  }
  


  int frag_size;
  if( ioctl(audio_fd, SNDCTL_DSP_GETBLKSIZE, &frag_size)==-1){
    perror("What's frag size");
  } 
  
  cout<<"Frag Size = "<<frag_size<<"\n";

	
}

/** play the samples*/
void SoundPlayer::playSamples(unsigned char* samples, int numSamples){
  write(audio_fd, samples, numSamples);
  //   flush(audio_fd);
}












