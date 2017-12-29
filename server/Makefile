CGI_DIR  = /usr/lib//cgi-bin/
HTML_DIR = /var/www/html

CGI_FILES = *.py *.php
HTML_FILES = styles.css

all:
	@echo Nothing to do

install: all
	sudo cp $(CGI_FILES) $(CGI_DIR)
	sudo cp $(HTML_FILES) $(HTML_DIR)

clean:
	@echo Nothing to do
