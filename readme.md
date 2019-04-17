
The WAV header contains some recognisable text info.
```bash
$ xxd tmp/long.raw | head -2
00000000: 5249 4646 ccba 0600 5741 5645 666d 7420  RIFF....WAVEfmt
00000010: 1000 0000 0100 0100 44ac 0000 8858 0100  ........D....X..

                               ^
                               |
ac44 is 44100Hz in hex --------|
```

```
cat /proc/asound/pcm

xxd /dev/snd/pcmC0D0c
xxd: File descriptor in bad state

cat /dev/input/by-path/platform-i8042-serio-0-event-kbd
xxd /dev/input/event0
```

ALSA dev
```
apt install libasound2-dev -y
```
Link with -lasound

- https://soundprogramming.net/programming/alsa-tutorial-1-initialization/

