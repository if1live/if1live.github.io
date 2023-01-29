ffmpeg -i video-crosh.webm -filter:v "crop=500:100:0:280" -y -pix_fmt rgb8 video-crosh.gif

ffmpeg -i video-secure-shell.webm -filter:v "crop=500:100:0:280" -y -pix_fmt rgb8 video-secure-shell.gif

ffmpeg -i video-ttyd-locale-fail.webm -filter:v "crop=450:150:0:110" -y -pix_fmt rgb8 video-ttyd-locale-fail.gif

ffmpeg -i video-ttyd.webm -filter:v "crop=450:170:0:0" -y -pix_fmt rgb8 video-ttyd.gif
