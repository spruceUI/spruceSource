docker build -t pyui_miyoomini .

mkdir output
docker run --rm -v ${PWD}/output:/output pyui_miyoomini cp set_monitor /output/
docker run --rm -v ${PWD}/output:/output pyui_miyoomini cp send_event /output/
docker run --rm -v ${PWD}/output:/output pyui_miyoomini cp getevent /output/
docker run --rm -v ${PWD}/output:/output pyui_miyoomini cp axp_read /output/

Charge screen frames (every Mini), one set per screen size, from a spruceOS checkout:
pip install pillow
python3 make_charge_frames.py --size 752x560 <spruceOS>/Themes/SPRUCE/skin_752x560 <spruceOS>/spruce/miyoomini/charge/752x560
python3 make_charge_frames.py --size 640x480 <spruceOS>/Themes/SPRUCE/skin <spruceOS>/spruce/miyoomini/charge/640x480
