#define _DEFAULT_SOURCE
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<stdint.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/stat.h>
#include<linux/mman.h>
#include<math.h>
#include<sys/syscall.h>

#define __u64 uint64_t

int cachestat(unsigned int fd, struct cachestat_range *cstat_range,
    struct cachestat *cstat, unsigned int flags) {
  return syscall(SYS_cachestat, fd, cstat_range, cstat, flags);
}

void show_cachestat(const char* filename) {
  int fd = open(filename, O_RDONLY);
  if(fd < 0) {
    fprintf(stderr, "ERROR open %s: %s\n", filename, strerror(errno));
    return;
  }

  struct stat s;
  int ret = fstat(fd, &s);
  if(ret < 0) {
    fprintf(stderr, "ERROR fstat %s: %s\n", filename, strerror(errno));
    close(fd);
    return;
  }

  off_t len = s.st_size;
  uint64_t pages = ceil((double)len / (double)sysconf(_SC_PAGE_SIZE));

  struct cachestat_range range = {
    .off = 0, .len = 0,
  };
  struct cachestat st;
  ret = cachestat(fd, &range, &st, 0);
  if(ret == 0) {
    printf(
      "%s cached %lld (%.1f%%) dirty %lld writeback %lld evicted %lld recently_evicted %lld\n",
      filename,
      st.nr_cache, (double)st.nr_cache / (double)pages * 100.0,
      st.nr_dirty,
      st.nr_writeback,
      st.nr_evicted,
      st.nr_recently_evicted
    );
  } else {
    fprintf(stderr, "ERROR cachestat %s: %s\n", filename, strerror(errno));
  }
  close(fd);
}

int main(int argc, char **argv){
  for(int i=1; i<argc; i++) {
    show_cachestat(argv[i]);
  }
  return 0;
}
