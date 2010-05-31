/*
* SoundPlayer.h
* 
* Play 16 bit, 44kHz samples to the speakers
*/


class SoundPlayer{
  
 private:
  int audio_fd;
	
	
 public:
  
  /** construct a new SoundPlayer*/
  SoundPlayer(){ audio_fd = 0;}
  
  /** play 16 bit samples, BE*/
  void playSamples( unsigned char* samples, int sampleLength );
  
  /** set up /dev/dsp to play samples at 44kHz, 16bit, mono*/
  void setUpAudio();
};

