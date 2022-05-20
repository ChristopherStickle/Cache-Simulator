//
// Created by stickle on 4/27/2022.
//
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<stdbool.h>

typedef struct {
    bool valid;
    unsigned long long tag;
    int replacementCounter;
}set_line;

typedef struct {
    set_line* line;
}cache_set;

struct {
    cache_set* set;
}cache;

unsigned long long address_array[2000]; //An array to hold all instruction values from user
char hit_miss_array[2000]; //An array to hold all h or m tags for the instruction array
char replacement_policy[3]; //holds the replacement policy chosen y the user

int S,s; //S=#of sets in the cache, s=#of set bits
int E; //#of lines in each set
int m; //#of bits in each address
int B,b,t; //B=Block-offset, b=#of block-offset bis, t=#of tag bits
int h,mp,cycle_count=0,miss_count=0,total_addresses=0; //h=hit time, mp=miss penalty time

//derives # of bits in set, block, tag---set all valid bits to 0
void BuildCache(){
    s=log2(S); //#of set bits
    b=log2(B); //#of off-set bits
    t=m-s-b;   //#of tag bits

    //malloc cache
    //Malloc the outline of the cache with S sections
    cache.set = (cache_set*)malloc(S*sizeof(cache_set));

    //Inside each set section, malloc E lines and initialize valid and replacementCounter
    for(int i=0;i<S;i++){
        cache_set set;
        set.line = (set_line*)malloc(E*sizeof(set_line));
        cache.set[i] = set;

        for(int j=0;j<E;j++){
            //set all valid and replacement bits to 0
            cache.set[i].line[j].valid=0;
            cache.set[i].line[j].replacementCounter=0;
        }
    }
}

//Called if miss occurs, load mem tag into cache, evict if necessary
void loadCache(unsigned int set_num,unsigned long long tag_num,int index){
    for(int j=0; j < E; j++){
        //Look for an empty line to store tag
        //if empty line, store tag, set valid bit, set replacement counter
        if(!cache.set[set_num].line[j].valid){
            cache.set[set_num].line[j].tag=tag_num;
            cache.set[set_num].line[j].valid=1;
            if(replacement_policy[1]=='r'||replacement_policy[1]=='R'){
                cache.set[set_num].line[j].replacementCounter=index;
            }
            if(replacement_policy[1]=='f'||replacement_policy[1]=='F'){
                cache.set[set_num].line[j].replacementCounter++;
            }
            return;
        }
    }
    //evict something and store tag, set valid bit, set replacement counter
    int least = cache.set[set_num].line[0].replacementCounter;
    int least_index = 0;
    for (int i = 1; i < E; i++) {
        if(cache.set[set_num].line[i].replacementCounter<least){
            least = cache.set[set_num].line[i].replacementCounter;
            least_index = i;
        }
    }
    cache.set[set_num].line[least_index].tag=tag_num;
    cache.set[set_num].line[least_index].valid=1;
    if(replacement_policy[1]=='r'||replacement_policy[1]=='R'){
        cache.set[set_num].line[least_index].replacementCounter=index;
    }
    if(replacement_policy[1]=='f'||replacement_policy[1]=='F'){
        cache.set[set_num].line[least_index].replacementCounter=1;
    }
}

//Read one set for valid && tag, update replacement counters
void ReadCache(unsigned int set_num,unsigned long long tag_num,int index){
    for(int i=0;i<E;i++){
        if(cache.set[set_num].line[i].valid && cache.set[set_num].line[i].tag==tag_num){
            //hit--adj. cycle_count, and hit_miss_array, replacementCounter
            if(replacement_policy[1]=='r'||replacement_policy[1]=='R'){
                cache.set[set_num].line[i].replacementCounter=index;
            }
            if(replacement_policy[1]=='f'||replacement_policy[1]=='F'){
                cache.set[set_num].line[i].replacementCounter++;
            }
            cycle_count = cycle_count + h;
            hit_miss_array[index]='h';
            return;
        }
    }
    //miss--adj. miss_count, cycle_count, hit_miss_array, call loadCache()
    miss_count++;
    cycle_count = cycle_count + h + mp;
    hit_miss_array[index]='m';
    loadCache(set_num,tag_num,index);
}

//prints mem array, hit_miss array, miss rate and cycle_count
void DisplayInfo(){
    for(int i = 0; i < total_addresses; i++){
        printf("%llx %c\n", address_array[i], hit_miss_array[i]);
    }
    //printf("misses: %d  total addresses: %d\n",miss_count,total_addresses); //echo check
    double miss_rate = (((double)miss_count / (double)total_addresses) * 100.0);
    printf("%d %d\n",(int)round(miss_rate),cycle_count);
}

//Free everything
void FreeCache() {
    for(int j=S-1;j>=0;j--) {
        cache_set set = cache.set[j];
        free(set.line);
    }
    free(cache.set);
}

int main() {
    //Inputs from user-------------------------------------------------------------------------------------------------
    printf("Please enter 'S E B m' values: \n");
    scanf("%d", &S);
    scanf("%d", &E);
    scanf("%d", &B);
    scanf("%d", &m);
    printf("Please chose a replacement policy (LFU|LRU): \n");
    scanf("%3s", &replacement_policy);
    printf("Please enter the hit time and miss penalty: \n");
    scanf("%d", &h);
    scanf("%d", &mp);
    printf("Please enter the addresses you wish to 'read', enter '-1' to begin simulation: \n");

    //copy '0x' addresses into array for later use ends with '-1'
    unsigned long long value=0;
    int i=0;
    while(value != -1){
        scanf("%0llx",&value);
        address_array[i]=value;
        i++;
    }
    //End of inputs from user------------------------------------------------------------------------------------------
    total_addresses=(i-1); //effective length of array excluding '-1' tail element, saved for later use.
    if(total_addresses==0){ printf("No addresses entered.\n");exit(0);}
    BuildCache();
    //printf("s,m,b,t: %d, %d, %d, %d\n",s,m,b,t); //LOG2-echo check

    //Loop through address_array---------------------------------------------------------------------------------------
    //for each element, bit shift the tag and set, pass them into readCache()
    unsigned long long address,tag_num;
    unsigned int set_num;
    for(int j = 0; j<total_addresses;j++){
        address=address_array[j];
        tag_num=(address >> (s+b));
        set_num=((address >> b)&((1<<s)-1));
        //printf("address: %llx  tag: %llx  set: %u\n",address,tag_num,set_num);  //BITSHIFT-echo check
        ReadCache(set_num,tag_num,j);
    }
    //End Loop---------------------------------------------------------------------------------------------------------
    DisplayInfo();
    FreeCache();
    return 0;
}
