
def print_mem_trace(mem_trace):
	print ">>>>>>>>>>>>>>>>>>[loaded mem_trace]<<<<<<<<<<<<<<<<<<<<<<<<"
	for ad in mem_trace:
		print "=addr:", ad, "thread_num", mem_trace[ad]["thread_num"], "tid_list", mem_trace[ad]["tid_list"]
		for tid in mem_trace[ad]:
			if tid != "thread_num" and tid != "tid_list":
				print "==>tid", tid, "optag", mem_trace[ad][tid]["optag"]
				for it in mem_trace[ad][tid]["trace_list"]:
					print "====>", it 

def print_ci(first, second, inter):
	print "-----------------------"
	print "first:", first
	print "second:", second
	print "inter:", inter
	print "-----------------------"

def print_ci_list(ci_list):
	print ">>>>>>>>>>>>>>>>>>[calculated CI list]<<<<<<<<<<<<<<<<<<<<<<<<"
	for i in ci_list:
		print "first:",i["first"]
		print "second:",i["second"]
		print "inter:",i["inter"]
		print "-----------------------"

def print_result_list(result_list):
	print ">>>>>>>>>>>>>>>>>>[result list after prunning]<<<<<<<<<<<<<<<<<<<<<<<<"
	for i in result_list:
		print "first:",i["first"]
		print "second:",i["second"]
		print "inter:",i["inter"]
		print "first_cs:",i["first_cs"]
		print "second_cs:",i["second_cs"]
		print "inter_cs:",i["inter_cs"]
		print "-----------------------"

def print_cs_list(cs_list):
	print ">>>>>>>>>>>>>>>>>>[loaded cs list]<<<<<<<<<<<<<<<<<<<<<<<<"
	for i in cs_list:
		print i

def print_sync_list(sync_list):
	print ">>>>>>>>>>>>>>>>>>[loaded sync list]<<<<<<<<<<<<<<<<<<<<<<<<"
	for i in sync_list:
		print i

def show_statistics(mem_trace,cs_list,sync_list,ci_list,result_list,group_num):
	print ">>>>>>>>>>>>>>>>>>[static result]<<<<<<<<<<<<<<<<<<<<<<<<"

	total_access = 0
	for addr in mem_trace:
		tid_list = mem_trace[addr]["tid_list"]
		#print "addr:",addr
		for i in range(len(tid_list)):
			total_access = total_access + len(mem_trace[addr][str(tid_list[i])]["trace_list"])
			#print "\ttid:",tid_list[i],", count:",len(mem_trace[addr][str(tid_list[i])]["trace_list"]), ", optag:",mem_trace[addr][str(tid_list[i])]["optag"]


	print "total accesses:",total_access
	print "visited addresses num:", len(mem_trace)
	#print "cs list length:", len(cs_list)
	#print "sync list length:", len(sync_list)
	print "total CI num:", len(ci_list)
	print "result CI num after prunning:", len(result_list)
	print "group num:", group_num
	
