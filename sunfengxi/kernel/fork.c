#include<stdio.h>
#include<stdlib.h>
#include <sched.h>

long last_pid=0;

int copy_process(int nr)
{
	struct task_struct *p;
	int i;
	struct file *f;

	p = (struct task_struct *) malloc(sizeof(struct task_struct));
	if (!p){
	      printf("\tOut of memory !");			
		return -1;
	}
	task[nr] = p;
	*p = *current;	
	p->state = TASK_UNINTERRUPTIBLE;
	p->pid = last_pid;
	p->father = current->pid;
	p->counter = p->priority;
	p->signal = 0;
	p->alarm = 0;
	p->leader = 0;		
	p->utime = p->stime = 0;
	p->cutime = p->cstime = 0;
	p->start_time = jiffies;	
	p->state = TASK_RUNNING;	/* do this last, just in case */
	return last_pid;
}





int find_empty_process(void)
{
	int i;

	repeat:
		if ((++last_pid)<0) last_pid=1;
		for(i=0 ; i<NR_TASKS ; i++)
			if (task[i] && task[i]->pid == last_pid) goto repeat;
	for(i=1 ; i<NR_TASKS ; i++)
		if (!task[i])
			return i;
	return -1;
}

/*
 *为新生成的进程分配所需的资源并进行初始化工作和出错处理
 *
 *
 *
 *
 *
 */
long do_fork(unsigned long clone_flags,
	      unsigned long stack_start,
	      struct pt_regs *regs,
	      unsigned long stack_size,
	      int __user *parent_tidptr,
	      int __user *child_tidptr)
{
	struct task_struct *p;
	int trace = 0;
	long pid = alloc_pidmap();/*为子进程申请进程号*/

	if (pid < 0)
		return -EAGAIN;/*系统资源不足，稍后再试*/
	/*	
	if (unlikely(current->ptrace)) {
		trace = fork_traceflag (clone_flags);
		if (trace)
			clone_flags |= CLONE_PTRACE;
	}
        */
	p = copy_process(clone_flags, stack_start, regs, stack_size, parent_tidptr, child_tidptr, pid);
	
	if (!IS_ERR(p)) {
		struct completion vfork;

		if (clone_flags & CLONE_VFORK) {
			p->vfork_done = &vfork;
			init_completion(&vfork);
		}

		if ((p->ptrace & PT_PTRACED) || (clone_flags & CLONE_STOPPED)) {
			
			sigaddset(&p->pending.signal, SIGSTOP);
			set_tsk_thread_flag(p, TIF_SIGPENDING);
		}

		if (!(clone_flags & CLONE_STOPPED))
			wake_up_new_task(p, clone_flags);
		else
			p->state = TASK_STOPPED;

		if (unlikely (trace)) {
			current->ptrace_message = pid;
			ptrace_notify ((trace << 8) | SIGTRAP);
		}

		if (clone_flags & CLONE_VFORK) {
			wait_for_completion(&vfork);
			if (unlikely (current->ptrace & PT_TRACE_VFORK_DONE))
				ptrace_notify ((PTRACE_EVENT_VFORK_DONE << 8) | SIGTRAP);
		}
	} else {
		free_pidmap(pid);
		pid = PTR_ERR(p);
	}
	return pid;
}


static task_t *copy_process(unsigned long clone_flags,
				 unsigned long stack_start,
				 struct pt_regs *regs,
				 unsigned long stack_size,
				 int __user *parent_tidptr,
				 int __user *child_tidptr,
				 int pid)
{
	int retval;
	struct task_struct *p = NULL;

	if ((clone_flags & (CLONE_NEWNS|CLONE_FS)) == (CLONE_NEWNS|CLONE_FS))
		return ERR_PTR(-EINVAL);

	/*
	 * Thread groups must share signals as well, and detached threads
	 * can only be started up within the thread group.
	 */
	if ((clone_flags & CLONE_THREAD) && !(clone_flags & CLONE_SIGHAND))
		return ERR_PTR(-EINVAL);

	/*
	 * Shared signal handlers imply shared VM. By way of the above,
	 * thread groups also imply shared VM. Blocking this case allows
	 * for various simplifications in other code.
	 */
	if ((clone_flags & CLONE_SIGHAND) && !(clone_flags & CLONE_VM))
		return ERR_PTR(-EINVAL);

	retval = security_task_create(clone_flags);
	if (retval)
		goto fork_out;

	retval = -ENOMEM;
	p = dup_task_struct(current);
	if (!p)
		goto fork_out;

	retval = -EAGAIN;
	if (atomic_read(&p->user->processes) >=
			p->signal->rlim[RLIMIT_NPROC].rlim_cur) {
		if (!capable(CAP_SYS_ADMIN) && !capable(CAP_SYS_RESOURCE) &&
				p->user != &root_user)
			goto bad_fork_free;
	}

	atomic_inc(&p->user->__count);
	atomic_inc(&p->user->processes);
	get_group_info(p->group_info);

	/*
	 * If multiple threads are within copy_process(), then this check
	 * triggers too late. This doesn't hurt, the check is only there
	 * to stop root fork bombs.
	 */
	if (nr_threads >= max_threads)
		goto bad_fork_cleanup_count;

	if (!try_module_get(task_thread_info(p)->exec_domain->module))
		goto bad_fork_cleanup_count;

	if (p->binfmt && !try_module_get(p->binfmt->module))
		goto bad_fork_cleanup_put_domain;

	p->did_exec = 0;
	copy_flags(clone_flags, p);
	p->pid = pid;
	retval = -EFAULT;
	if (clone_flags & CLONE_PARENT_SETTID)
		if (put_user(p->pid, parent_tidptr))
			goto bad_fork_cleanup;

	p->proc_dentry = NULL;

	INIT_LIST_HEAD(&p->children);
	INIT_LIST_HEAD(&p->sibling);
	p->vfork_done = NULL;
	spin_lock_init(&p->alloc_lock);
	spin_lock_init(&p->proc_lock);

	clear_tsk_thread_flag(p, TIF_SIGPENDING);
	init_sigpending(&p->pending);

	p->utime = cputime_zero;
	p->stime = cputime_zero;
 	p->sched_time = 0;
	p->rchar = 0;		/* I/O counter: bytes read */
	p->wchar = 0;		/* I/O counter: bytes written */
	p->syscr = 0;		/* I/O counter: read syscalls */
	p->syscw = 0;		/* I/O counter: write syscalls */
	acct_clear_integrals(p);

 	p->it_virt_expires = cputime_zero;
	p->it_prof_expires = cputime_zero;
 	p->it_sched_expires = 0;
 	INIT_LIST_HEAD(&p->cpu_timers[0]);
 	INIT_LIST_HEAD(&p->cpu_timers[1]);
 	INIT_LIST_HEAD(&p->cpu_timers[2]);

	p->lock_depth = -1;		/* -1 = no lock */
	do_posix_clock_monotonic_gettime(&p->start_time);
	p->security = NULL;
	p->io_context = NULL;
	p->io_wait = NULL;
	p->audit_context = NULL;
#ifdef CONFIG_NUMA
 	p->mempolicy = mpol_copy(p->mempolicy);
 	if (IS_ERR(p->mempolicy)) {
 		retval = PTR_ERR(p->mempolicy);
 		p->mempolicy = NULL;
 		goto bad_fork_cleanup;
 	}
#endif

	p->tgid = p->pid;
	if (clone_flags & CLONE_THREAD)
		p->tgid = current->tgid;

	if ((retval = security_task_alloc(p)))
		goto bad_fork_cleanup_policy;
	if ((retval = audit_alloc(p)))
		goto bad_fork_cleanup_security;
	/* copy all the process information */
	if ((retval = copy_semundo(clone_flags, p)))
		goto bad_fork_cleanup_audit;
	if ((retval = copy_files(clone_flags, p)))
		goto bad_fork_cleanup_semundo;
	if ((retval = copy_fs(clone_flags, p)))
		goto bad_fork_cleanup_files;
	if ((retval = copy_sighand(clone_flags, p)))
		goto bad_fork_cleanup_fs;
	if ((retval = copy_signal(clone_flags, p)))
		goto bad_fork_cleanup_sighand;
	if ((retval = copy_mm(clone_flags, p)))
		goto bad_fork_cleanup_signal;
	if ((retval = copy_keys(clone_flags, p)))
		goto bad_fork_cleanup_mm;
	if ((retval = copy_namespace(clone_flags, p)))
		goto bad_fork_cleanup_keys;
	retval = copy_thread(0, clone_flags, stack_start, stack_size, p, regs);
	if (retval)
		goto bad_fork_cleanup_namespace;

	p->set_child_tid = (clone_flags & CLONE_CHILD_SETTID) ? child_tidptr : NULL;
	/*
	 * Clear TID on mm_release()?
	 */
	p->clear_child_tid = (clone_flags & CLONE_CHILD_CLEARTID) ? child_tidptr: NULL;

	/*
	 * Syscall tracing should be turned off in the child regardless
	 * of CLONE_PTRACE.
	 */
	clear_tsk_thread_flag(p, TIF_SYSCALL_TRACE);
#ifdef TIF_SYSCALL_EMU
	clear_tsk_thread_flag(p, TIF_SYSCALL_EMU);
#endif

	/* Our parent execution domain becomes current domain
	   These must match for thread signalling to apply */
	   
	p->parent_exec_id = p->self_exec_id;

	/* ok, now we should be set up.. */
	p->exit_signal = (clone_flags & CLONE_THREAD) ? -1 : (clone_flags & CSIGNAL);
	p->pdeath_signal = 0;
	p->exit_state = 0;

	/*
	 * Ok, make it visible to the rest of the system.
	 * We dont wake it up yet.
	 */
	p->group_leader = p;
	INIT_LIST_HEAD(&p->ptrace_children);
	INIT_LIST_HEAD(&p->ptrace_list);

	/* Perform scheduler related setup. Assign this task to a CPU. */
	sched_fork(p, clone_flags);

	/* Need tasklist lock for parent etc handling! */
	write_lock_irq(&tasklist_lock);

	/*
	 * The task hasn't been attached yet, so its cpus_allowed mask will
	 * not be changed, nor will its assigned CPU.
	 *
	 * The cpus_allowed mask of the parent may have changed after it was
	 * copied first time - so re-copy it here, then check the child's CPU
	 * to ensure it is on a valid CPU (and if not, just force it back to
	 * parent's CPU). This avoids alot of nasty races.
	 */
	p->cpus_allowed = current->cpus_allowed;
	if (unlikely(!cpu_isset(task_cpu(p), p->cpus_allowed) ||
			!cpu_online(task_cpu(p))))
		set_task_cpu(p, smp_processor_id());

	/*
	 * Check for pending SIGKILL! The new thread should not be allowed
	 * to slip out of an OOM kill. (or normal SIGKILL.)
	 */
	if (sigismember(&current->pending.signal, SIGKILL)) {
		write_unlock_irq(&tasklist_lock);
		retval = -EINTR;
		goto bad_fork_cleanup_namespace;
	}

	/* CLONE_PARENT re-uses the old parent */
	if (clone_flags & (CLONE_PARENT|CLONE_THREAD))
		p->real_parent = current->real_parent;
	else
		p->real_parent = current;
	p->parent = p->real_parent;

	if (clone_flags & CLONE_THREAD) {
		spin_lock(&current->sighand->siglock);
		/*
		 * Important: if an exit-all has been started then
		 * do not create this new thread - the whole thread
		 * group is supposed to exit anyway.
		 */
		if (current->signal->flags & SIGNAL_GROUP_EXIT) {
			spin_unlock(&current->sighand->siglock);
			write_unlock_irq(&tasklist_lock);
			retval = -EAGAIN;
			goto bad_fork_cleanup_namespace;
		}
		p->group_leader = current->group_leader;

		if (current->signal->group_stop_count > 0) {
			/*
			 * There is an all-stop in progress for the group.
			 * We ourselves will stop as soon as we check signals.
			 * Make the new thread part of that group stop too.
			 */
			current->signal->group_stop_count++;
			set_tsk_thread_flag(p, TIF_SIGPENDING);
		}

		if (!cputime_eq(current->signal->it_virt_expires,
				cputime_zero) ||
		    !cputime_eq(current->signal->it_prof_expires,
				cputime_zero) ||
		    current->signal->rlim[RLIMIT_CPU].rlim_cur != RLIM_INFINITY ||
		    !list_empty(&current->signal->cpu_timers[0]) ||
		    !list_empty(&current->signal->cpu_timers[1]) ||
		    !list_empty(&current->signal->cpu_timers[2])) {
			/*
			 * Have child wake up on its first tick to check
			 * for process CPU timers.
			 */
			p->it_prof_expires = jiffies_to_cputime(1);
		}

		spin_unlock(&current->sighand->siglock);
	}

	/*
	 * inherit ioprio
	 */
	p->ioprio = current->ioprio;

	SET_LINKS(p);
	if (unlikely(p->ptrace & PT_PTRACED))
		__ptrace_link(p, current->parent);

	attach_pid(p, PIDTYPE_PID, p->pid);
	attach_pid(p, PIDTYPE_TGID, p->tgid);
	if (thread_group_leader(p)) {
		attach_pid(p, PIDTYPE_PGID, process_group(p));
		attach_pid(p, PIDTYPE_SID, p->signal->session);
		if (p->pid)
			__get_cpu_var(process_counts)++;
	}

	if (!current->signal->tty && p->signal->tty)
		p->signal->tty = NULL;

	nr_threads++;
	total_forks++;
	write_unlock_irq(&tasklist_lock);
	proc_fork_connector(p);
	cpuset_fork(p);
	retval = 0;

fork_out:
	if (retval)
		return ERR_PTR(retval);
	return p;

bad_fork_cleanup_namespace:
	exit_namespace(p);
bad_fork_cleanup_keys:
	exit_keys(p);
bad_fork_cleanup_mm:
	if (p->mm)
		mmput(p->mm);
bad_fork_cleanup_signal:
	exit_signal(p);
bad_fork_cleanup_sighand:
	exit_sighand(p);
bad_fork_cleanup_fs:
	exit_fs(p); /* blocking */
bad_fork_cleanup_files:
	exit_files(p); /* blocking */
bad_fork_cleanup_semundo:
	exit_sem(p);
bad_fork_cleanup_audit:
	audit_free(p);
bad_fork_cleanup_security:
	security_task_free(p);
bad_fork_cleanup_policy:
#ifdef CONFIG_NUMA
	mpol_free(p->mempolicy);
#endif
bad_fork_cleanup:
	if (p->binfmt)
		module_put(p->binfmt->module);
bad_fork_cleanup_put_domain:
	module_put(task_thread_info(p)->exec_domain->module);
bad_fork_cleanup_count:
	put_group_info(p->group_info);
	atomic_dec(&p->user->processes);
	free_uid(p->user);
bad_fork_free:
	free_task(p);
	goto fork_out;
}

struct pt_regs * __devinit __attribute__((weak)) idle_regs(struct pt_regs *regs)
{
	memset(regs, 0, sizeof(struct pt_regs));
	return regs;
}

task_t * __devinit fork_idle(int cpu)
{
	task_t *task;
	struct pt_regs regs;

	task = copy_process(CLONE_VM, 0, idle_regs(&regs), 0, NULL, NULL, 0);
	if (!task)
		return ERR_PTR(-ENOMEM);
	init_idle(task, cpu);
	unhash_process(task);
	return task;
}

static inline int fork_traceflag (unsigned clone_flags)
{
	if (clone_flags & CLONE_UNTRACED)
		return 0;
	else if (clone_flags & CLONE_VFORK) {
		if (current->ptrace & PT_TRACE_VFORK)
			return PTRACE_EVENT_VFORK;
	} else if ((clone_flags & CSIGNAL) != SIGCHLD) {
		if (current->ptrace & PT_TRACE_CLONE)
			return PTRACE_EVENT_CLONE;
	} else if (current->ptrace & PT_TRACE_FORK)
		return PTRACE_EVENT_FORK;

	return 0;
}











