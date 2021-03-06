#include <visual.h>
#include <audio.h>

void setDefaultSpec(SDL_AudioSpec &spec) {
      SDL_zero(spec);
      spec.freq = RATE;
      spec.channels = 2;
      spec.samples = SAMPLES;
      spec.format = AUDIO_S16;
}

void wavCallBack(void *userData, Uint8 *stream, int len) {
      WavData *audio = (WavData *)userData;
      if (audio->BufferByteSize == 0) return;

      Uint32 length = len;
      visualizerOutput(stream, audio->format);

      length = (length > audio->BufferByteSize ? audio->BufferByteSize : length);

      memcpy(stream, audio->Buffer, length);

      audio->Buffer += length;
      audio->BufferByteSize -= length;
}

void musicMode(char *file_stream) {
      if (file_stream == NULL) {
            file_stream = musicUI();
      }
      if (file_stream == NULL) {
            puts("NO FILE SELECTED");
            return;
      }
      SDL_AudioSpec wav_spec;
      SDL_AudioSpec obtained;
      Uint8 *wav_buffer;
      Uint32 wav_length;
      WavData *audio = (WavData *)malloc(sizeof(WavData));
      if (SDL_LoadWAV(file_stream, &wav_spec, &wav_buffer, &wav_length) == NULL) {
            fprintf(stderr, "FILE ERROR: %s\n", SDL_GetError());
            exit(1);
      }

      audio->Buffer = wav_buffer;
      audio->BufferByteSize = wav_length;
      setDefaultSpec(wav_spec);
      wav_spec.callback = wavCallBack;
      wav_spec.userdata = audio;
      audio->format = wav_spec.format;
      SDL_AudioDeviceID playDeviceId = 0;
      playDeviceId = SDL_OpenAudioDevice(NULL, 0, &wav_spec, &obtained, SDL_AUDIO_ALLOW_FORMAT_CHANGE);

      if (playDeviceId == 0) {
            printf("Failed to open recording device! SDL Error: %s", SDL_GetError());
            exit(1);
      }

      SDL_PauseAudioDevice(playDeviceId, SDL_FALSE);
      bool quit = false, stop = false, pause = false;
      while (!quit) {
            int height, width;
            SDL_GetWindowSize(window, &width, &height);
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                  if (event.type == SDL_QUIT) quit = true, finish = true;
                  if (event.type == SDL_MOUSEBUTTONDOWN) {
                        int xx, yy;
                        SDL_GetMouseState(&xx, &yy);
                        if (circleIntersects(xx, yy, pauserect)) {
                              if (!stop) {
                                    surf = SDL_CreateRGBSurface(0, width, height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
                                    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, surf->pixels, surf->pitch);
                                    pauseTex = SDL_CreateTextureFromSurface(renderer, surf);
                                    pause ^= 1;
                                    SDL_PauseAudioDevice(playDeviceId, pause);
                              } else {
                                    stop = false;
                                    audio->Buffer = wav_buffer;
                                    audio->BufferByteSize = wav_length;
                                    setDefaultSpec(wav_spec);
                                    wav_spec.callback = wavCallBack;
                                    wav_spec.userdata = audio;
                                    audio->format = wav_spec.format;
                                    playDeviceId = SDL_OpenAudioDevice(NULL, 0, &wav_spec, &obtained, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
                                    SDL_PauseAudioDevice(playDeviceId, SDL_FALSE);
                              }
                        }
                        if (circleIntersects(xx, yy, stoprect)) {
                              quit = 1;
                              break;
                        }
                        if (rectangleIntersects(xx, yy, moderect) && !pause) {
                              changeMode();
                        }
                  }
                  else if(event.type == SDL_KEYDOWN) {
                        switch (event.key.keysym.sym) {
                        case SDLK_m:
                              changeMode();
                              break;
                        case SDLK_q:
                              quit = 1;
                              break;
                        case SDLK_SPACE:
                              if(!stop) {
                                    surf = SDL_CreateRGBSurface(0, width, height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
                                    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, surf->pixels, surf->pitch);
                                    pauseTex = SDL_CreateTextureFromSurface(renderer, surf);
                                    pause ^= 1;
                                    SDL_PauseAudioDevice(playDeviceId, pause);
                              }
                              else {
                                    stop = false;
                                    audio->Buffer = wav_buffer;
                                    audio->BufferByteSize = wav_length;
                                    setDefaultSpec(wav_spec);
                                    wav_spec.callback = wavCallBack;
                                    wav_spec.userdata = audio;
                                    audio->format = wav_spec.format;
                                    playDeviceId = SDL_OpenAudioDevice(NULL, 0, &wav_spec, &obtained, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
                                    SDL_PauseAudioDevice(playDeviceId, SDL_FALSE); 
                              }
                              break;
                        default:
                              break;
                        }
                  }
            }
            if (pause) {
                  pauserect.x = width / 100, pauserect.y = height / 100;
                  pauserect.w = min(width, height) / 10, pauserect.h = min(width, height) / 10;

                  stoprect.x = width / 100 + pauserect.w + width / 100, stoprect.y = height / 100;
                  stoprect.w = min(width, height) / 10, stoprect.h = min(width, height) / 10;
                  
                  if(pauseTex == NULL) puts("pauseTex failed");
                  SDL_RenderClear(renderer);
                  SDL_RenderCopy(renderer, pauseTex, NULL, NULL);
                  SDL_RenderCopy(renderer, tplay, NULL, &pauserect);
                  SDL_RenderCopy(renderer, tstop, NULL, &stoprect);
                  SDL_RenderPresent(renderer);
            }
            if (((WavData *)wav_spec.userdata)->BufferByteSize == 0 && !stop) {
                  SDL_PauseAudioDevice(playDeviceId, true);
                  stop = true;

            }
            if (stop) {
                  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                  SDL_RenderClear(renderer);

                  pauserect.x = width / 100, pauserect.y = height / 100;
                  pauserect.w = min(width, height) / 10, pauserect.h = min(width, height) / 10;
                  stoprect.x = width / 100 + pauserect.w + width / 100, stoprect.y = height / 100;
                  stoprect.w = min(width, height) / 10, stoprect.h = min(width, height) / 10;

                  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                  SDL_RenderFillRect(renderer, &pauserect);
                  SDL_RenderDrawRect(renderer, &pauserect);

                  SDL_RenderCopy(renderer, tstop, NULL, &stoprect);
                  SDL_RenderCopy(renderer, treplay, NULL, &pauserect);
                  
                  SDL_RenderPresent(renderer);
            }
      }
      SDL_FreeWAV(wav_buffer);
      SDL_CloseAudioDevice(playDeviceId);
      return;
}
