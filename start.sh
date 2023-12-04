#!/bin/bash

cp -f /etc/ldap-account-manager/config/*.sample /usr/share/ldap-account-manager/config/
cp -f /etc/ldap-account-manager/config/language /usr/share/ldap-account-manager/config/language
cp -f /etc/ldap-account-manager/config/.htaccess /usr/share/ldap-account-manager/config/.htaccess
cp -rf /etc/ldap-account-manager/config/pdf /usr/share/ldap-account-manager/config/pdf
cp -rf /etc/ldap-account-manager/config/profiles /usr/share/ldap-account-manager/config/profiles
cp -rf /etc/ldap-account-manager/config/selfService /usr/share/ldap-account-manager/config/selfService
cp -rf /etc/ldap-account-manager/config/templates /usr/share/ldap-account-manager/config/templates
chown -R nobody /usr/share/ldap-account-manager/sess
chown -R nobody /usr/share/ldap-account-manager/tmp
chown -R nobody /usr/share/ldap-account-manager/config
chown nobody /etc/ldap-account-manager/config.cfg
sed -i 's/location \/lam/location \/'${LAM_NGINX_PATH}'/g' /etc/ldap-account-manager/nginx.conf
sed -i 's/listen 8080/listen '${LAM_LISTEN_PORT}'/g' /etc/nginx/http.d/default.conf
sed -i 's/listen \[\:\:\]\:8080/listen \[\:\:\]\:'${LAM_LISTEN_PORT}'/g' /etc/nginx/http.d/default.conf
supervisord -n -c /etc/supervisor/supervisord.conf
