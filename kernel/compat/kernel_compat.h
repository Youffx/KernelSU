#ifndef __KSU_H_KERNEL_COMPAT
#define __KSU_H_KERNEL_COMPAT

#include <linux/fs.h>
#include <linux/version.h>
#include <linux/task_work.h>
#include <linux/uaccess.h>

extern struct file *ksu_filp_open_compat(const char *filename, int flags,
					 umode_t mode);
extern ssize_t ksu_kernel_read_compat(struct file *p, void *buf, size_t count,
				      loff_t *pos);
extern ssize_t ksu_kernel_write_compat(struct file *p, const void *buf,
				       size_t count, loff_t *pos);

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 9, 0) && !defined(KSU_HAS_PATH_MOUNT)
extern int path_mount(const char *dev_name, struct path *path,
		      const char *type_page, unsigned long flags,
		      void *data_page);
#endif

extern long ksu_copy_from_user_nofault(void *dst, const void __user *src,
				       size_t size);

static inline long ksu_copy_from_user_retry(void *to,
		const void __user *from, unsigned long count)
{
	long ret = ksu_copy_from_user_nofault(to, from, count);
	if (likely(!ret))
		return ret;
	return copy_from_user(to, from, count);
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 12, 0)
extern void *ksu_compat_kvrealloc(const void *p, size_t oldsize, size_t newsize,
				  gfp_t flags);
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 12, 0)
static inline void *ksu_kvmalloc(size_t size, gfp_t flags)
{
	void *buf = kmalloc(size, flags);
	if (!buf)
		buf = vmalloc(size);
	return buf;
}

static inline void ksu_kvfree(const void *buf)
{
	if (is_vmalloc_addr(buf))
		vfree(buf);
	else
		kfree(buf);
}
#define kvmalloc ksu_kvmalloc
#define kvfree ksu_kvfree
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 0, 0)
#define ksu_access_ok(addr, size) access_ok(addr, size)
#else
#define ksu_access_ok(addr, size) access_ok(VERIFY_READ, addr, size)
#endif

// strncpy_from_user_nofault: kernel has it natively since ~4.14
#if !defined(KSU_OPTIONAL_STRNCPY) && LINUX_VERSION_CODE < KERNEL_VERSION(4, 14, 0)
extern long strncpy_from_user_nofault(char *dst, const void __user *unsafe_addr,
				   long count);
#endif

// task_work_add: bool on <5.7, enum on >=5.7
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 7, 0)
#ifndef TWA_RESUME
#define TWA_RESUME true
#endif
#endif

#endif // #ifndef __KSU_H_KERNEL_COMPAT
