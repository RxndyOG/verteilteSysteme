#include <stdio.h>
#include <arpa/inet.h> // für htons()

int main() {
    unsigned short host_port = 8008;  // Port in Netzwerkbyte-Reihenfolge (z.B. 8080 in Hex)
    unsigned long host_ip = 19216801;

    unsigned short net_port = 0x481f;
    unsigned long net_ip = 0xA1392501;  // IP-Adresse in Netzwerkbyte-Reihenfolge (z.B. 192.168.0.1 in Hex)
    
    

    // Konvertiere von Netzwerkbyte-Reihenfolge in Hostbyte-Reihenfolge
    unsigned short net_port_test = htons(host_port);
    unsigned short host_port_test = ntohs(net_port);
    unsigned long host_ip_test = ntohl(net_ip);
    unsigned long net_ip_test = htonl(host_ip);

    printf("Netzwerk-Port: 0x%x (Host-Port: %d)\n", net_port_test, host_port);
    printf("Host-Port: %d (Netzwerk-Port: 0x%x)\n", host_port_test, net_port);
    

    printf("Netzwerk-IP: 0x%lX (Host-IP: %lu)\n", net_ip_test, host_ip);
    printf("Host-IP: %lu (Netzwerk-IP: 0x%lX)\n", host_ip_test, net_ip);
   

    return 0;
}
