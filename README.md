# Ldap Account Manager Free 7.4 dockerized repo.

To use in production:
```
cd /opt
git clone https://github.com/drlight17/lam-dockerized
cd lam-dockerized
docker-compose up -d
```
Then go to http://yourhostnameorip:8080/lam and use LAM.

- Saved configs will be in created ./config directory
- Saved sessions will be in created ./sess directory
- Saved temp files will be in created ./tmp directory

If you want to use/make some LAM customizations uncomment all binds in docker-compose.yaml. You can customize files from ./custom directory as you want.

To stop LAM run:
```
docker-compose down
```
## For delelopers and image builders: use docker-compose.yaml and Dockerfiles from the appropiate folders of this repo instead of default deploy docker-compose.yaml!
