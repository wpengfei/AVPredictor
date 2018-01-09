
def print_mem_trace(trace_mem):
	for ad in trace_mem:
		print "=addr:", ad, "thread_num", trace_mem[ad]["thread_num"], "tid_list", trace_mem[ad]["tid_list"]
		for tid in trace_mem[ad]:
			if tid != "thread_num" and tid != "tid_list":
				print "==>tid", tid, "optag", trace_mem[ad][tid]["optag"]
				for it in trace_mem[ad][tid]["trace_list"]:
					print "====>", it 

def print_ci(first, second, inter):
	print "-----------------------"
	print "first:", first
	print "second:", second
	print "inter:", inter
	print "-----------------------"

def print_ci_list(ci_list):
	print "ci_list  =========================================="
	for i in ci_list:
		print "first:",i["first"]
		print "second:",i["second"]
		print "inter:",i["inter"]
		print "-----------------------"

def print_result_list(result_list):
	print "result_list  =========================================="
	for i in result_list:
		print "first:",i["first"]
		
		print "second:",i["second"]
		
		print "inter:",i["inter"]
		print "first_cs:",i["first_cs"]
		print "second_cs:",i["second_cs"]
		print "inter_cs:",i["inter_cs"]
		print "-----------------------"

def print_cs_list(cs_list):
	print "cs_list result =========================================="
	for i in cs_list:
		print i

