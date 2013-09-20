#ifndef __STATUS_H__
#define __STATUS_H__

struct Status {
	bool ok = true;
	char error[32] = "ok";
};

#endif /* __STATUS_H__ */
