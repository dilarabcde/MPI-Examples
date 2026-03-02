/*
n adet eleman içeren bir dizinin p adet işlemciye dağıtılması için yük dengelemeyi dikkate alan aşağıdaki algoritmadan
faydalanılır. (İlgili sunudaki Algoritma 2)

Adım 1: Dizinin boyutu olan n n=q∙p+r formunda ifade edilir. Burada q bölüm ve r de kalandır.
        C++ dilinde q değeri n/p ve r değeri de n%p ile hesaplanır.
Adım 2: İlk r işlemcinin her birine ceil(n/p) kadar veri, geriye kalan p-r işlemcinin her birine de floor(n/p) kadar veri atanır.

Örneğin n=23 ve p=7 iken 23=3∙7+2 dir, yani q=3 ve r=2'dir. Bu durumda:
İlk 2 işlemcinin her biri ceil(23/7)=4 adet veri alır.
Geriye kalan 7-2=5 işlemcinin her birinin payına floor(23/7)=3 adet veri düşer.
P0->4, P1->4, P2->3, P3->3, P4->3, P5->3, P6->3
*/

#include <mpi.h>
#include <iostream>//cout,cin
#include <cstdlib>//rand,srand
#include <ctime>//time(0)
#include <cmath>//floor,ceil
using namespace std;

#define TAG 25

int ArrayMaximum(int*,int);//Prototip
void PrintArray(int*,int);//Prototip

int main(int argc,char *argv[])
{
    srand((unsigned int)time(0));

    int myRank,
        size,
            diziBoyutu,//Dinamik Dizinin Boyutu
            kismiBoyut,//Her Bir İşlemcinin Payına Düşecek Veri Miktarı
            quotient,//Bölüm (q)
            remainder,//Kalan (r)
            ceiledValue,//ceil(n/p)
            flooredValue,//floor(n/p)
        adresSayaci,
        globalMaksimum,
        lokalMaksimum;

    int *diziPtr,//Verisi Paylaştırılacak Dinamik Dizi
            *kismiDiziPtr,//Her Bir İşlemcinin Aldığı Veriyi Depolayacağı Dinamik Dizi
            *veriMiktarlariPtr,//Her Bir İşlemciye Gönderilecek Veri Miktarını Saklayacak Dinamik Dizi
            *adreslerPtr;//Her Bir İşlemciye Gönderilecek Verinin diziPtr İçerisindeki Adres Bilgisini (Başlangıç Adresini) Saklayacak Dinamik Dizi

    MPI_Status status;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&myRank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    if(myRank==0)//Master Process
    {
        //ADIM 1: Kullanıcıdan Dizinin Boyutunu Öğren
        cout<<"Dinamik Dizi Boyutu?: ";
        cin>>diziBoyutu;

        //ADIM 2: Dizinin Elemanlarını Depolamak İçin Dinamik Bir Bellek Alanı Oluştur
        diziPtr=new int[diziBoyutu];//Dinamik Diziyi Yok Etmeyi Unutma

        //ADIM 3: Dizinin İçini Rastgele Sayılarla Doldur
        for(int i=0;i<diziBoyutu;i++)
            diziPtr[i]=rand()%2020;

        //ADIM 4: Küçük Boyutlu Dizilerde Diziyi Test Amaçlı Ekrana Yazdır
        PrintArray(diziPtr,diziBoyutu);

        //ADIM 5: Veri Paylaştırma Algoritmasının İhtiyaç Duyduğu Hesaplamaları Yap (n=qxp+r)
                quotient=diziBoyutu/size;//q=n/p
                remainder=diziBoyutu%size;//r=n%p
                ceiledValue=(int)ceil((double)diziBoyutu/size);
                flooredValue=(int)floor((double)diziBoyutu/size);

        //ADIM 6: Veri Paylaştırma Algoritmasının İhtiyaç Duyduğu Dinamik Dizileri Oluştur ve İçlerini Doldur
        adresSayaci=0;
                veriMiktarlariPtr=new int[size];
                adreslerPtr=new int[size];

                for(int i=0;i<size;i++)
                {
                        adreslerPtr[i]=adresSayaci;

                        if(i<remainder)
                        {
                                veriMiktarlariPtr[i]=ceiledValue;
                                adresSayaci+=ceiledValue;
                        }
                        else
                        {
                                veriMiktarlariPtr[i]=flooredValue;
                                adresSayaci+=flooredValue;
                        }
                }

        //ADIM 7: Yardımcı Dizilerin İçeriğini Test Amaçlı Ekrana Yazdır
        PrintArray(veriMiktarlariPtr,size);
        PrintArray(adreslerPtr,size);

        //ADIM 8: Dizinin Boyutunu Tüm İşlemcilere Gönder
        for(int i=1;i<size;i++)
                        MPI_Send(&diziBoyutu,1,MPI_INT,i,TAG,MPI_COMM_WORLD);

        //ADIM 9: Her Bir İşlemciye Payına Düşen Veriyi Gönder
        for(int i=1;i<size;i++)
            MPI_Send(&diziPtr[adreslerPtr[i]],veriMiktarlariPtr[i],MPI_INT,i,TAG,MPI_COMM_WORLD);

        //ADIM 10: Master İşlemcinin Payına Düşen Verinin Maksimumunu Bul
        globalMaksimum=ArrayMaximum(diziPtr,veriMiktarlariPtr[0]);

        //ADIM 11: Diğer İşlemcilerden Lokal Maksimumları Al ve Bu Lokal Maksimumların Maksimumunu Bul
        for(int i=1;i<size;i++)
        {
            MPI_Recv(&lokalMaksimum,1,MPI_INT,i,TAG,MPI_COMM_WORLD,&status);
            if(lokalMaksimum>globalMaksimum)
                globalMaksimum=lokalMaksimum;
        }

        //ADIM 12: Dizinin Maksimumunu Ekrana Yazdır
        cout<<"\nDizinin Maksimumu: "<<globalMaksimum<<endl;

        //ADIM 13: Dinamik Olarak Oluşturduğun Dizi Alanlarını Yok Et
        delete [] diziPtr;
        delete [] veriMiktarlariPtr;
        delete [] adreslerPtr;
    }
    else
    {
        //ADIM 1: Dizinin Boyutunu Master İşlemciden Al
        MPI_Recv(&diziBoyutu,1,MPI_INT,0,TAG,MPI_COMM_WORLD,&status);

        //ADIM 2: Veri Paylaştırma Algoritmasının İhtiyaç Duyduğu Hesaplamaları Yap (n=qxp+r)
            quotient=diziBoyutu/size;//q=n/p
            remainder=diziBoyutu%size;//r=n%p
            ceiledValue=(int)ceil((double)diziBoyutu/size);
            flooredValue=(int)floor((double)diziBoyutu/size);

        //ADIM 3: Payına Düşecek Kısmi Veri Miktarını Hesapla
        if(myRank<remainder)
            kismiBoyut=ceiledValue;
        else
            kismiBoyut=flooredValue;

        //ADIM 4: Payına Düşecek Veriyi Depolayacağın Dinamik Dizi Alanını Oluştur
        kismiDiziPtr=new int[kismiBoyut];//Dinamik Diziyi Yok Etmeyi Unutma

        //ADIM 5: Payına Düşen Veriyi Master İşlemciden Al
        MPI_Recv(kismiDiziPtr,kismiBoyut,MPI_INT,0,TAG,MPI_COMM_WORLD,&status);

        //ADIM 6: Küçük Boyutlu Dizilerde Aldığın Veriyi Test Amaçlı Ekrana Yazdır
        if(myRank==(size-1))
            PrintArray(kismiDiziPtr,kismiBoyut);

        //ADIM 7: Payına Düşen Verinin Maksimumunu (Lokal Maksimum) Bul
        lokalMaksimum=ArrayMaximum(kismiDiziPtr,kismiBoyut);

        //ADIM 8: Bulduğun Lokal Maksimumu Master İşlemciye Gönder
        MPI_Send(&lokalMaksimum,1,MPI_INT,0,TAG,MPI_COMM_WORLD);

        //ADIM 9: Dinamik Olarak Oluşturduğun Kısmi Dizi Alanını Yok Et
        delete [] kismiDiziPtr;
    }

    MPI_Finalize();

    return 0;
}

int ArrayMaximum(int *diziPtr,int diziBoyutu)
{
    int maximum=diziPtr[0];

    for(int i=1;i<diziBoyutu;i++)
        if(diziPtr[i]>maximum)
            maximum=diziPtr[i];

    return maximum;
}

void PrintArray(int *diziPtr,int diziBoyutu)
{
    for(int i=0;i<diziBoyutu;i++)
        cout<<diziPtr[i]<<" ";

    cout<<endl;
}
