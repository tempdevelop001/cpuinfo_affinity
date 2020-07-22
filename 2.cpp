﻿#include <windows.h>

#include <iostream>
#include <conio.h>


//---------------------------START
#if !defined(__arm__) && !defined(__aarch64__)
static inline void cpuid(int functionnumber, int output[4]) {
#ifdef _MSC_VER
    // Microsoft compiler, intrin.h included
    
#elif defined(__INTEL_COMPILER)
    __cpuid(output, functionnumber);
#elif defined(__GNUC__) || defined(__clang__)
    // use inline assembly, Gnu/AT&T syntax
    int a, b, c, d;
    asm volatile("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(functionnumber), "c"(0));
    output[0] = a;
    output[1] = b;
    output[2] = c;
    output[3] = d;
#else
    // unknown platform. try inline assembly with masm/intel syntax
    __asm {
        mov eax, functionnumber
        xor ecx, ecx
        cpuid;
        mov esi, output
        mov[esi], eax
        mov[esi + 4], ebx
        mov[esi + 8], ecx
        mov[esi + 12], edx
    }
#endif
}
#else /* !__arm__ */
#define cpuid(fn, out) out[0] = 0;
#endif

void cpu_get(char *outbuf, size_t maxsz)
{
    memset(outbuf, 0, maxsz);
#ifdef WIN32
    char brand[0xC0] = { 0 };
    int output[4] = { 0 }, ext;
    cpuid(0x80000000, output);
    ext = output[0];
    if (ext >= 0x80000004) {
	//for (int i = 2; i <= (ext & 0xF); i++) {
	for (int i=2; i<5; i++) {	
	    cpuid(0x80000000+i, output);
	    memcpy(&brand[(i-2) * 4*sizeof(int)], output, 4*sizeof(int));
	}
	snprintf(outbuf, maxsz, "%s", brand);
    } else {
	// Fallback, for the i7-5775C will output
	// Intel64 Family 6 Model 71 Stepping 1, GenuineIntel
	snprintf(outbuf, maxsz, "%s", getenv("PROCESSOR_IDENTIFIER"));
    }
#else
    // Intel(R) Xeon(R) CPU E3-1245 V2 @ 3.40GHz
    FILE *fd = fopen("/proc/cpuinfo", "rb");
    char *buf = NULL, *p, *eol;
    size_t size = 0;
    if (!fd) return;
    while(getdelim(&buf, &size, 0, fd) != -1) {
	if (buf && (p = strstr(buf, "model name\t")) && strstr(p, ":")) {
	    p = strstr(p, ":");
	    if (p) {
		p += 2;
		eol = strstr(p, "\n"); if (eol) *eol = '\0';
		snprintf(outbuf, maxsz, "%s", p);
	    }
	    break;
	}
    }
    free(buf);
    fclose(fd);
#endif
}
//---------------------------END




int main(void){

    std::cout<<"Hello World!";
    std::cout<<"\n";



//определяем кол-во процессоров, только общее кол-во потоков
static int num_processors;    
#if defined(WIN32)
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	num_processors = sysinfo.dwNumberOfProcessors;
#elif defined(_SC_NPROCESSORS_CONF)
	num_processors = sysconf(_SC_NPROCESSORS_CONF);
#elif defined(CTL_HW) && defined(HW_NCPU)
	int req[] = { CTL_HW, HW_NCPU }; //HW_NCPU to HW_AVAILCPU
	size_t len = sizeof(num_processors);
	sysctl(req, 2, &num_processors, &len, NULL, 0);
#else
	num_processors = 1;
#endif
	if (num_processors < 1)
		num_processors = 1;



    std::cout<<"Cpus: ";
    std::cout<<num_processors;
    std::cout<<"\n";
//----------------END


// Opredelenice cpu



//----------------------S
#define BUFFERLEN 128
#ifndef WIN32
#include <sys/sysctl.h>
#endif /* !WIN32 */

char cpuname[80] = { 0 };
cpu_get(cpuname, 80);

    #ifndef WIN32
    char name_cpu[BUFFERLEN];
    size_t bufferlen = BUFFERLEN;
    sysctlbyname("machdep.cpu.brand_string",&name_cpu,&bufferlen,NULL,0);
    #endif /* !WIN32 */


printf("Info: %s\n", cpuname);
//----------------------E

    
//    args << "-P";



    getch();
    return 0;
}




