#include "utils.h"

enum state {
      startscreen,
      recording,
      recorded,
      startplaying,
      playing,
      doneplaying
};


void createDefaultRecData(int MAX_RECORDING_SECONDS, SDL_AudioSpec ReceivedRecordingSpec) {
      int bytesPerSample = ReceivedRecordingSpec.channels * (SDL_AUDIO_BITSIZE(ReceivedRecordingSpec.format) / 8);
      int bytesPerSecond = ReceivedRecordingSpec.freq * bytesPerSample;
      recData.BufferByteSize = (1 + MAX_RECORDING_SECONDS) * bytesPerSecond;
      recData.BufferByteMaxPosition = MAX_RECORDING_SECONDS * bytesPerSecond;
      recData.Buffer = (Uint8 *)malloc(sizeof(Uint8) * recData.BufferByteSize);
      memset(recData.Buffer, 0, recData.BufferByteSize);
      recData.BufferBytePosition = 0;
}

void startRecording() {
      clearRenderer();
      SDL_AudioSpec desiredRecordingSpec;
      setDefaultSpec(desiredRecordingSpec);
      desiredRecordingSpec.callback = RecordingCallBack;
      recordingDeviceId = SDL_OpenAudioDevice(NULL, SDL_TRUE, &desiredRecordingSpec, &ReceivedRecordingSpec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
      if (recordingDeviceId == 0) {
            printf("Failed to open recording device! SDL Error: %s", SDL_GetError());
            exit(1);
      }
      SDL_AudioSpec desiredPlaybackSpec;
      SDL_zero(desiredPlaybackSpec);
      setDefaultSpec(desiredPlaybackSpec);
      desiredPlaybackSpec.callback = PlaybackCallBack;
      playbackDeviceId = SDL_OpenAudioDevice(NULL, SDL_FALSE, &desiredPlaybackSpec, &ReceivedPlaybackSpec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
      if (playbackDeviceId == 0) {
            printf("Failed to open recording device! SDL Error: %s", SDL_GetError());
            exit(1);
      }
      createDefaultRecData(MAX_RECORDING_SECONDS, ReceivedRecordingSpec);
      SDL_PauseAudioDevice(recordingDeviceId, SDL_FALSE);
}

void recordMode() {
      int st = 0, timer = 0;
      state currentState=startscreen;
      bool quit = false, pause = false;
      load_rec_UI();
      while (!quit) {
            int height, width;
            SDL_GetWindowSize(window, &width, &height);

            if (currentState == recording) {
                  if (pause)
                        rec_UI(-1, st);
                  else
                        rec_UI(currentState, st);
            } else
                  rec_UI(currentState, st);
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                  if (event.type == SDL_QUIT) quit = true;
                  if (currentState == startscreen) {
                        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_s) {
                              startRecording();
                              currentState=recording;
                        }
                  }
                  if (currentState == recorded) {
                        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_n) {
                              currentState = recording;
                              createDefaultRecData(MAX_RECORDING_SECONDS, ReceivedRecordingSpec);
                              SDL_PauseAudioDevice(recordingDeviceId, SDL_FALSE);
                        } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE)
                              currentState=startplaying;
                  }
                  if (currentState == startplaying) {
                        recData.BufferBytePosition = 0;
                        SDL_PauseAudioDevice(playbackDeviceId, SDL_FALSE);
                        currentState=playing;
                  }
                  if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_m) changeMode();
                  if (event.type == SDL_MOUSEBUTTONDOWN) {
                        int xx, yy;
                        SDL_GetMouseState(&xx, &yy);
                        if (cir_intersects(xx, yy, pauserect)) {
                              pause ^= 1;
                              if (currentState == recording)
                                    SDL_PauseAudioDevice(recordingDeviceId, pause);
                              else if (currentState == playing){
                                    surf = SDL_CreateRGBSurface(0, width, height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
                                    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, surf->pixels, surf->pitch);
                                    pauseTex = SDL_CreateTextureFromSurface(renderer, surf);
                                    SDL_PauseAudioDevice(playbackDeviceId, pause);
                              }      
                              else if (currentState == doneplaying) {
                                    currentState = startplaying;
                                    pause = false;
                              }
                        }
                        if (cir_intersects(xx, yy, stoprect)) {
                              if (currentState == recording)
                                    goto outter;
                              else if (currentState == playing) {
                                    quit = 1;
                                    break;
                              }
                        }
                  }
            }
            if (currentState == recording) {
                  SDL_LockAudioDevice(recordingDeviceId);
                  if (recData.BufferBytePosition > recData.BufferByteMaxPosition) {
                  outter:
                        recData.BufferByteMaxPosition = recData.BufferBytePosition;
                        SDL_PauseAudioDevice(recordingDeviceId, SDL_TRUE);
                        pause = false;
                        currentState=recorded;
                  }
                  SDL_UnlockAudioDevice(recordingDeviceId);
            }
            if (currentState == playing) {
                  SDL_LockAudioDevice(playbackDeviceId);
                  if (recData.BufferBytePosition > recData.BufferByteMaxPosition) {
                        surf = SDL_CreateRGBSurface(0, width, height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
                        SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, surf->pixels, surf->pitch);
                        pauseTex = SDL_CreateTextureFromSurface(renderer, surf);
                        SDL_PauseAudioDevice(playbackDeviceId, SDL_TRUE);
                        currentState=doneplaying;
                  }
                  SDL_UnlockAudioDevice(playbackDeviceId);
            }
            if (pause && currentState == playing) {
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

            if (currentState == doneplaying) {
                  //puts("replay render");
                  //SDL_RenderClear(renderer);
                  pauserect.x = width / 100, pauserect.y = height / 100;
                  pauserect.w = min(width, height) / 10, pauserect.h = min(width, height) / 10;

                  if(pauseTex == NULL) puts("pauseTex failed");
                  SDL_RenderClear(renderer);
                  SDL_RenderCopy(renderer, pauseTex, NULL, NULL);

                  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                  SDL_RenderFillRect(renderer, &pauserect);
                  SDL_RenderDrawRect(renderer, &pauserect);
                  SDL_RenderCopy(renderer, treplay, NULL, &pauserect);
                  SDL_RenderPresent(renderer);
            }

            timer = (timer + 1) % 25;
            if (timer == 24) st = (st + 1) % 4;
      }

      SDL_CloseAudioDevice(recordingDeviceId);
      SDL_CloseAudioDevice(playbackDeviceId);
}
