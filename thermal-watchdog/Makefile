 DOCKER_IMAGE := thermal_watchdog
 BINARY := "thermal-watchdog"
 
 all: build
 
 build:
	 docker build -t $(DOCKER_IMAGE) .
	 docker create --name $(DOCKER_IMAGE) $(DOCKER_IMAGE) true
	 docker cp $(DOCKER_IMAGE):/$(BINARY) .
 
 clean:
	 docker rmi --force thermal_watchdog
	 docker rm --force thermal_watchdog
	 rm -f $(BINARY)