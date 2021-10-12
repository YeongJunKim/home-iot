### Raspberry Pi zero w USB setup



### Install ReSpeaker 2-Mic Hat Driver

``` bash
$ git clone https://github.com/respeaker/seeed-voicecard
$ cd seeed-voicecard
$ sudo ./install.sh 
$ sudo reboot
```

### snowboy
**`npm install` 로 여럿 시도해 보았지만 안됨**
https://pimylifeup.com/raspberry-pi-snowboy/ : not work well
https://jackpang.tistory.com/33 : section 3.0
``` bash
$ sudo apt-get install swig
```
``` bash
$ sudo apt install python3 python3-pyaudio python3-pip libatlas-base-dev portaudio19-dev
```
``` bash
$ sudo pip3 install pyaudio
```
``` bash
$ sudo pip3 install requests
```
``` bash
$ git clone https://github.com/kitt-ai/snowboy
$ cd snowboy/swig/
$ make
```
Test
``` bash
$ cd snowboy/example/Python3
$ python3 demo.py /resource/models/snowboy.umdl
```
If occur error message with 
```
Traceback (most recent call last):

  File "demo.py", line 1, in <module>

    import snowboydecoder

  File "/home/pi/snowboy/examples/Python3/snowboydecoder.py", line 5, in <module>

    from . import snowboydetect

ImportError: attempted relative import with no known parent package
```
then, edit below
``` bash
$ sudo nano snowboydecoder.py
```
|Before|After|
|-----|-----|
|from . import snowboydetect | import snowboydetect





#### Install
``` bash
npm install --save snowboy
```
If there is an error with "npm does not support Node.js v10.15.2",
``` bash
curl https://www.npmjs.com/install.sh | sudo sh
```
