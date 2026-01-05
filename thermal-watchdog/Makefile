 DOCKER_IMAGE := thermal_watchdog
 BINARY := "thermal-watchdog"
 
 all: build
 
 build:
	 docker build -t $(DOCKER_IMAGE) .
	 docker create --name $(DOCKER_IMAGE) $(DOCKER_IMAGE) true
	 docker cp $(DOCKER_IMAGE):/$(BINARY) .
 
 clean:
	 docker rmi --force $(DOCKER_IMAGE)
	 docker rm --force $(DOCKER_IMAGE)
	 rm -f $(BINARY)