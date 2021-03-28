#include<stdio.h>


int main(){
	FILE *conf;
	conf = fopen("/etc/samba/smb.conf","a");
	fprintf(conf, "[Entrada]\n   path = /var/ocr-server/Entrada\n   browseable = yes\n   read only = no\n   guest ok = yes\n");
	fprintf(conf, "[Saida]\n   path = /var/ocr-server/Saida\n   browseable = yes\n   read only = no\n   guest ok = yes\n");
	fprintf(conf, "[Originais Processados]\n   path = /var/ocr-server/Originais_Processados\n    browseable = yes\n   read only = no\n   guest ok = yes\n");
	fprintf(conf, "[Erro]\n   path = /var/ocr-server/Erro\n   browseable = yes\n   read only = no\n   guest ok = yes\n");
	
	return 0;
}
