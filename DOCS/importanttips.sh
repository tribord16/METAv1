# pour deplacer frontend a nginx 
sudo cp -r build/* /var/www/mon-app2/

#pour relancer le front
sudo nginx -t && sudo systemctl reload nginx