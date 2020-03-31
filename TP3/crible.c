#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h>
void afficher(char* crible, int n)
{
    printf("nombres premiers jusqu'a %d\n",n);
    int i=2;
    for(;i<n;i++){
        if(crible[i]==1){
            printf("%d ",i);        
        }    
    }
    printf("\n");
}

void rayer_multiples(char* crible, int n, int k)
{
    int i=2;
    //printf("k=%d\n",k);
    while(i*k<=n){
       //printf("i=%d\n",i);
       //printf("i*k=%d\n",i*k);
       crible[i*k]=0;
       i++;        
        
    }
}


int main(int argc, char **argv)
{
    int n=1000;
    if(argc>1)
    {
        n = atoi(argv[1]);
        assert( n > 0 );
    }

    scanf("%d",&n);

    //char buffer[1000];
    //assert(n <= 1000);
    
    char *crible=mmap(NULL,n,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,0,0);

    
    //char * crible = (char*)&buffer ;

    for(int i=0; i<n; i++)
    {
        crible[i] = 1;//par défaut: pas encore barré
    }
    
    for(int k=2; k<n; k++)
    {
        //rayer_multiples(crible, n, k);
        //fork duplique le contexte courant l'enfant aura la bonne valeur de k
        if(!fork()){
            rayer_multiples(crible, n, k);
            exit(0);      
        }
        
        int i=n-2;
        for(;i>0;i--){
            wait(NULL);        
        };
        
    }
    afficher(crible, n);
    
    
    return 0;
}
