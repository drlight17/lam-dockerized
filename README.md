# Ldap Account Manager Free 7.4 dockerized repo.

To use in production:
```
cd /opt
git clone https://github.com/drlight17/lam-dockerized
cd lam-dockerized
cp example.env .env
```

Make sure to change default parametres if you want in .env file before next command!

```
docker-compose up -d
```

Then go to http://yourhostnameorip:8080/lam (in case of default settings in .env file) and use LAM .

- Saved configs will be in created ./config directory
- Saved sessions will be in created ./sess directory
- Saved temp files will be in created ./tmp directory

If you want to use/make some LAM customizations uncomment needed binds in docker-compose.yaml. You can customize files from ./custom directory as you want.

To stop LAM run:
```
docker-compose down
```

### For delelopers and image builders: use docker-compose.yaml and Dockerfiles from the appropiate folders of this repo instead of default deploy docker-compose.yaml!

Also place all files from repo's ./etc and ./usr dirs in your /opt/lam-dockerized!

./usr/share/ldap-account-manager.zip must be unpacked into ./usr/share/ldap-account-manager (it is packed due to the limits of github).
