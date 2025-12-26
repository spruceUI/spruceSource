docker build -t idlemon_arm7 .

mkdir compiled-armhf
docker run --rm -v ${PWD}/output:/output idlemon_arm7 cp idlemon /compiled-armhf/