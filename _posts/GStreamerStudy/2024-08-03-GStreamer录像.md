gst-launch-1.0 v4l2src device="/dev/video0" ! videoconvert !  nvvideoconvert ! tee name=t \
t. ! nvh264enc ! h264parse ! qtmux ! filesink location=video.mp4 \
t. ! nveglglessink

gst-launch-1.0 -e v4l2src ! videoconvert ! nvvideoconvert ! queue ! timeoverlay ! nvh264enc ! h264parse ! splitmuxsink location=video%02d.mkv max-size-time=10000000000 muxer-factory=matroskamux muxer-properties="properties,streamable=true"