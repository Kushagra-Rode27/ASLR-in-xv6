#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "elf.h"
#include "date.h" //added by us
// static int loaduvm(pde_t *pgdir, char *addr, struct inode *ip, uint offset, uint sz);

int
exec(char *path, char **argv)
{
  char *s, *last;
  int i, off;
  uint argc, sz, sp, ustack[3+MAXARG+1];
  struct elfhdr elf;
  struct inode *ip;
  struct proghdr ph;
  pde_t *pgdir, *oldpgdir;
  struct proc *curproc = myproc();

  struct rtcdate *r = 0;   // current time

  begin_op();
  // Get aslr_fl flag
  int aslr_fl = 0;
  char c;
  if ((ip = namei("aslr_flag")) == 0) {
    cprintf("unable to open aslr_flag file\n");
  } 
  else {
    ilock(ip);
    if (readi(ip, &c, 0, sizeof(char)) != sizeof(char)) {
      cprintf("unable to read aslr_flag file\n");
    } else {
      if(c == '1'){
        aslr_fl = 1;
      }
      else{
        aslr_fl = 0;
      }
      // aslr_fl = (c == '1')? 1 : 0;
    }
    iunlockput(ip);
  }

  if((ip = namei(path)) == 0){
    end_op();
    cprintf("exec: fail\n");
    return -1;
  }
  ilock(ip);
  pgdir = 0;

  // Check ELF header
  if(readi(ip, (char*)&elf, 0, sizeof(elf)) != sizeof(elf))
    goto bad;
  if(elf.magic != ELF_MAGIC)
    goto bad;

  if((pgdir = setupkvm()) == 0)
    goto bad;

  // Load program into memory.
  sz = 0;

  cmostime(r);
  srand(r->second);
  int rnum = rand();
  int random_num = (rnum % 2) ? rnum : rnum + 1;
  // cprintf("1st random = %x\n",random_num);

  uint load_offset = (aslr_fl) ? random_num: 0;
  // Load program into memory.
  
  sz = allocuvm(pgdir, 0, load_offset);
  // cprintf("value of sz %x\n",sz);
  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
    if(readi(ip, (char*)&ph, off, sizeof(ph)) != sizeof(ph))
      goto bad;
    if(ph.type != ELF_PROG_LOAD)
      continue;
    if(ph.memsz < ph.filesz)
      goto bad;
    // if(ph.vaddr + ph.memsz < ph.vaddr)
    //   goto bad;
    if((sz = allocuvm(pgdir, sz, ph.vaddr + ph.memsz + load_offset)) == 0) {
      cprintf("exec: uvmalloc error\n");
      goto bad;
    }
    // if(ph.vaddr % PGSIZE != 0)
    //   goto bad;
    // if(loadseg(pgdir, ph.vaddr, ip, ph.off, ph.filesz) < 0)
    //   goto bad;
    if(loaduvm(pgdir, (char*)(ph.vaddr + load_offset), ip, ph.off, ph.filesz) < 0) {
      cprintf("exec: loaduvm error\n");
      goto bad;
    }
  }
  // cprintf("b after for loop in exec.c:exec");
  iunlockput(ip);
  end_op();
  ip = 0;

  // Allocate two pages at the next page boundary.
  // Make the first inaccessible.  Use the second as the user stack.
  // cprintf("b before stack_offset in exec.c:exec");

  cmostime(r);
  srand(r->second + 3);
  int rnum2 = rand();
  int random_num2 = (rnum2 % 2) ? rnum2 : rnum2 + 1;
  // cprintf("2nd random = %x\n",random_num2);
  int stack_offset = (aslr_fl)? random_num2 : 2;
  sz = PGROUNDUP(sz);
  if((sz = allocuvm(pgdir, sz, sz + stack_offset*PGSIZE)) == 0)
    goto bad;
  clearpteu(pgdir, (char*)(sz - 2*PGSIZE));
  sp = sz;
  // cprintf("b before setting sp in exec.c:exec");

  // Push argument strings, prepare rest of stack in ustack.
  for(argc = 0; argv[argc]; argc++) {
    if(argc >= MAXARG)
      goto bad;
    sp = (sp - (strlen(argv[argc]) + 1)) & ~3;
    if(copyout(pgdir, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
      goto bad;
    ustack[3+argc] = sp;
  }
  ustack[3+argc] = 0;

  ustack[0] = 0xffffffff;  // fake return PC
  ustack[1] = argc;
  ustack[2] = sp - (argc+1)*4;  // argv pointer

  sp -= (3+argc+1) * 4;
  if(copyout(pgdir, sp, ustack, (3+argc+1)*4) < 0)
    goto bad;

  // Save program name for debugging.
  for(last=s=path; *s; s++)
    if(*s == '/')
      last = s+1;
  safestrcpy(curproc->name, last, sizeof(curproc->name));

  // Commit to the user image.
  oldpgdir = curproc->pgdir;
  curproc->pgdir = pgdir;
  curproc->sz = sz;
  curproc->tf->eip = elf.entry + load_offset;  // main
  curproc->tf->esp = sp;
  switchuvm(curproc);
  freevm(oldpgdir);
  return 0;

 bad:
  if(pgdir)
    freevm(pgdir);
  if(ip){
    iunlockput(ip);
    end_op();
  }
  return -1;
}
