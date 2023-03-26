#if !defined(MP3TOWAV_H)
#define MP3TOWAV_H

void mp3ToWave_init(void);

void mp3ToWave_convert(char *mp3_path, char *wav_path);

void mp3ToWave_cleanup(void);

#endif // MP3TOWAV_H
