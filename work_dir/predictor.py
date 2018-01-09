import scanf
import tool

trace_mem = {} # structured memory traces
ci_list = [] # list of candidate cis
cs_list = [] # list of critical sections
sync_list = [] #list of sync primitives
result_list = []

group_num = 0
group_set = []

def load_sync_traces():
	file_sync_handler = open("file_sync.log", "r")
	each_line = True
	while each_line:
		each_line = file_sync_handler.readline()

		if each_line:
			tid, time, ty = scanf.sscanf(each_line,"tid:%d,time:%d,type:%s\n")

			sync = {}
			sync["tid"] = tid
			sync["type"] = ty
			sync["time"] = time
			sync_list.append(sync)

	file_sync_handler.close()



def load_lock_traces():
	file_lock_handler = open("file_lock.log", "r")
	each_line = True
	while each_line:
		each_line = file_lock_handler.readline()

		if each_line:
			tid, op, time, callsite_v, entry_v = scanf.sscanf(each_line,"tid:%d,op:%c,time:%d,callsite_v:0x%x,entry_v:0x%x\n")

			callsite_v = hex(callsite_v)
			entry_v = hex(entry_v)

			if op == "L": #create a critical section 
				cs = {} # three-tuple (tid, call_lock_v, call_unlock_v) can uniquely determine a critical section 
				cs["tid"] = str(tid)
				cs["stime"] = time #start time 
				cs["ftime"] = 0 # finish time
				cs["l_cv"] = callsite_v # lock_callsite_value
				cs["l_ev"] = entry_v # lock_entry_value
				cs["u_cv"] = 0 # unlock_callsite_value
				cs["u_ev"] = 0 # unlock_entry_value
				cs_list.append(cs)
			else: #complete the critical section
				for it in cs_list:
					if it["tid"] == str(tid) and it["l_ev"] == entry_v and it["ftime"] == 0:
						it["ftime"] = time
						it["u_cv"] = callsite_v
						it["u_ev"] = entry_v

	file_lock_handler.close()





 

'''
trace_mem = {"addr1": addr_dict1, "addr2":addr_dict2}

addr_dict = {"thread_num":2, "tid_list":tid_list, "tid1":tid_dict1, "tid2": tid_dict2}

tid_list = [tid1,tid2]

tid_dict = {"optag": 2, "trace_list": tlist}    # optag: 0,R; 1,W; 2,R&W

tlist = [trace1, trace2]



'''


def load_mem_traces():
	file_mem_handler = open("file_mem_access.log", "r")
	each_line = True
	while each_line:
		each_line = file_mem_handler.readline()
		#print each_line

		if each_line:
			
			tid, op, time, ip, addr, rtn = scanf.sscanf(each_line,"tid:%d,op:%c,time:%d,ip:0x%x,addr:0x%x,rtn:%s\n")	
			
			ip = str(hex(ip))
			addr = str(hex(addr))
			tid = str(tid)
			trace = {}
			trace["tid"] = tid
			trace["op"] = op 
			trace["time"] = time
			trace["ip"] = ip
			trace["addr"] = addr
			trace["rtn"] = rtn

			if  trace_mem.has_key(addr):

				if trace_mem[addr].has_key(tid):

					optag = trace_mem[addr][tid]["optag"]

					if  op == "R" and optag == 1:
						optag = 2;
						trace_mem[addr][tid]["optag"] = optag
					elif op == "W" and optag == 0:
						optag = 2;
						trace_mem[addr][tid]["optag"] = optag

					trace_mem[addr][tid]["trace_list"].append(trace)

				else:
					tlist = []
					tlist.append(trace)

					tid_dict = {}
					if op == "R":
						tid_dict["optag"] = 0
					else:
						tid_dict["optag"] = 1
					tid_dict["trace_list"] = tlist

					trace_mem[addr]["thread_num"] = trace_mem[addr]["thread_num"] + 1
					trace_mem[addr]["tid_list"].append(tid)
					trace_mem[addr][tid] = tid_dict

			else:
				tlist = []
				tlist.append(trace)

				tid_dict = {}
				if op == "R":
					tid_dict["optag"] = 0
				else:
					tid_dict["optag"] = 1
				tid_dict["trace_list"] = tlist

				tid_list = [];
				tid_list.append(tid)

				addr_dict = {}
				addr_dict["thread_num"] = 1
				addr_dict["tid_list"] = tid_list
				addr_dict[tid] = tid_dict
				
				trace_mem[addr] = addr_dict

	file_mem_handler.close()



def find_write_trace(tlist,time1,time2):
	assert time1 < time2
	ret = []
	for it in tlist:
		if it["time"] > time1 and it["time"] < time2:
			continue
		else:
			if it["op"] == "W" :
				ret.append(it)
	return ret

def find_read_trace(tlist,time1,time2):
	assert time1 < time2
	ret = []
	for it in tlist:
		if it["time"] > time1 and it["time"] < time2:
			continue
		else:
			if it["op"] == "R" :
				ret.append(it)
	return ret

# a "tid_dict" contains the memory traces that accessing the same addr from the same thread
def find_ci_from_thread_pair(tid_dict0, tid_dict1):
	if tid_dict0["optag"] == 0 and tid_dict1["optag"] == 0:
		return None
	elif tid_dict0["optag"] == 1 and tid_dict1["optag"] == 1:
		return None

	else: 
		tlist0 = tid_dict0["trace_list"]
		tlist1 = tid_dict1["trace_list"]

		l0 = len(tlist0)
		l1 = len(tlist1)

		if l0 >= 2:
			for i in range(l0-1):
				# first and second in a CI must not be interleaved by an access from the same thread
				# aka. first and second must be close to each other in the list
				if tlist0[i]["op"] == "R" and tlist0[i+1]["op"] == "R":
					ret_list = find_write_trace(tlist1,tlist0[i]["time"],tlist0[i+1]["time"])
					for it in ret_list:
						ci = {}
						ci["first"] = tlist0[i]
						ci["second"] = tlist0[i+1]
						ci["inter"] = it
						ci_list.append(ci)

				elif tlist0[i]["op"] == "W" and tlist0[i+1]["op"] == "R":
					ret_list = find_write_trace(tlist1,tlist0[i]["time"],tlist0[i+1]["time"])
					for it in ret_list:
						ci = {}
						ci["first"] = tlist0[i]
						ci["second"] = tlist0[i+1]
						ci["inter"] = it
						ci_list.append(ci)
				elif tlist0[i]["op"] == "R" and tlist0[i+1]["op"] == "W":
					ret_list = find_write_trace(tlist1,tlist0[i]["time"],tlist0[i+1]["time"])
					for it in ret_list:
						ci = {}
						ci["first"] = tlist0[i]
						ci["second"] = tlist0[i+1]
						ci["inter"] = it
						ci_list.append(ci)
				elif tlist0[i]["op"] == "W" and tlist0[i+1]["op"] == "W":
					ret_list = find_read_trace(tlist1,tlist0[i]["time"],tlist0[i+1]["time"])
					for it in ret_list:
						ci = {}
						ci["first"] = tlist0[i]
						ci["second"] = tlist0[i+1]
						ci["inter"] = it
						ci_list.append(ci)

		if l1 >= 2:
			for i in range(l1-1): # switch tlist0 and tlist 1
			
				if tlist1[i]["op"] == "R" and tlist1[i+1]["op"] == "R":
					ret_list = find_write_trace(tlist0,tlist1[i]["time"],tlist1[i+1]["time"])
					for it in ret_list:
						ci = {}
						ci["first"] = tlist1[i]
						ci["second"] = tlist1[i+1]
						ci["inter"] = it
						ci_list.append(ci)

				elif tlist1[i]["op"] == "W" and tlist1[i+1]["op"] == "R":
					ret_list = find_write_trace(tlist0,tlist1[i]["time"],tlist1[i+1]["time"])
					for it in ret_list:
						ci = {}
						ci["first"] = tlist1[i]
						ci["second"] = tlist1[i+1]
						ci["inter"] = it
						ci_list.append(ci)
				elif tlist1[i]["op"] == "R" and tlist1[i+1]["op"] == "W":
					ret_list = find_write_trace(tlist0,tlist1[i]["time"],tlist1[i+1]["time"])
					for it in ret_list:
						ci = {}
						ci["first"] = tlist1[i]
						ci["second"] = tlist1[i+1]
						ci["inter"] = it
						ci_list.append(ci)
				elif tlist1[i]["op"] == "W" and tlist1[i+1]["op"] == "W":
					ret_list = find_read_trace(tlist0,tlist1[i]["time"],tlist1[i+1]["time"])
					for it in ret_list:
						ci = {}
						ci["first"] = tlist1[i]
						ci["second"] = tlist1[i+1]
						ci["inter"] = it
						ci_list.append(ci)



def find_thread_paris_for_same_addr():

	for addr in trace_mem:
		if trace_mem[addr]["thread_num"] < 2:
			#print "thread num < 2"
			continue
		else:
			tid_list = trace_mem[addr]["tid_list"]
			l = len(tid_list)
			assert l >= 2
			if l == 2: # for most of the cases, only two threads involved
				tid_dict0 = trace_mem[addr][tid_list[0]]
				tid_dict1 = trace_mem[addr][tid_list[1]]

				find_ci_from_thread_pair(tid_dict0, tid_dict1)
						
			else:
				for i in range(l):
					tid_dict0 = trace_mem[addr][tid_list[i]]
					for j in range(i+1,l):
						tid_dict1 = trace_mem[addr][tid_list[j]]
						find_ci_from_thread_pair(tid_dict0, tid_dict1)



def prune():

	for ci in ci_list:
		first = ci["first"]
		second = ci["second"]
		inter = ci["inter"]

		assert first["tid"] == second["tid"]

		#if first["time"] + 1 == second["time"]: 
			#continue

		# check for locks
		found = False #not same critical section
		for cs in cs_list:
			entry_v = 0
			#if first and second are protected by the same critical section, 
            #and the remote access are also protected by a same lock, then it won't cause a atomicity violation 
			if first["tid"] == cs["tid"] and first["time"] > cs["stime"] and first["time"] < cs["ftime"] \
				and second["time"] > cs["stime"] and second["time"] < cs["ftime"]:
				entry_v = cs["l_ev"]
				#print "entry_v", entry_v
				for cs2 in cs_list:
					if inter["tid"] == cs2["tid"] and inter["time"] > cs2["stime"] and inter["time"] < cs2["ftime"] \
						and cs2["l_ev"] == entry_v:
						found = True;

		if found: # protected by the same lock, abandon
			#print_ci(first, second, inter) 
			continue

		# check for syncs

		# pattern FCI:
    	#
    	#  T1        T2
    	#           inter
    	# --sync--
    	#  first
    	#   ...
    	#  second
    
		if inter["time"] < first["time"]: 

			found = False
			for syn in sync_list:
				if syn["tid"] == first["tid"] and syn["time"] > inter["time"] and  syn["time"] < first["time"]:
					found =True
					print "FCI find sync: ", sync
					break

			if found: # restricted by sync, abandon
				continue


		# pattern BCI:
    	#
    	#  T1        T2
    	#  first
    	#   ...
    	#  second
    	#          --sync--
    	#           inter
		elif inter["time"] > second["time"]: 
			found = False
			for syn in sync_list:
				if syn["tid"] == inter["tid"] and syn["time"] > second["time"] and  syn["time"] < inter["time"]:
					found =True
					print "BCI find sync: ", sync
					break

			if found: # restricted by sync, abandon
				continue

		#save result
		result = {}
		result["first"] = first
		result["second"] = second
		result["inter"] = inter
		result["first_cs"] = None
		result["second_cs"] = None
		result["inter_cs"] = None

		for cs in cs_list:
			if first["time"] > cs["stime"] and first["time"] < cs["ftime"] and first["tid"] == cs["tid"]:
				result["first_cs"] = cs

			if second["time"] > cs["stime"] and second["time"] < cs["ftime"] and second["tid"] == cs["tid"]:
				result["second_cs"] = cs

			if inter["time"] > cs["stime"] and inter["time"] < cs["ftime"] and inter["tid"] == cs["tid"]:
				result["inter_cs"] = cs

		result_list.append(result)





def grouper(first_interval, second_interval, inter_interval, msg_str):
	global group_num

	#print "enter grouper\n"
	if group_num == 0:
		#group_fhandler = open("replay/group_0.log", "w")
		#group_fhandler.write(msg_str)
		#group_fhandler.close()	

		ci = {} #candidate interleaving (CI) represented by time intervals
		ci["first_interval"] = first_interval
		ci["second_interval"] = second_interval
		ci["inter_interval"] = inter_interval
		ci["msg"] = msg_str
		group = []
		group.append(ci)
		group_set.append(group)
		group_num = group_num + 1

		#print "init, group_num:", group_num
		#print ci
		#print msg_str

	else:
		insert_id = -1 # the id of the group to be inserted
		insert_group_size = 0

		for i in range(group_num):
			l = len(group_set[i])
			interfere = False
			for j in range(l): # check every ci in the group for interference
				f_interval = group_set[i][j]["first_interval"]
				s_interval = group_set[i][j]["second_interval"]
				i_interval = group_set[i][j]["inter_interval"]

				if first_interval[0] == f_interval[0] and first_interval[1] == f_interval[1] and \
					second_interval[0] == s_interval[0] and second_interval[1] == s_interval[1] and \
					inter_interval[0] == i_interval[0] and inter_interval[1] == i_interval[1]:
						return # duplicate report, abandon

				if first_interval[0]>f_interval[1] and first_interval[0]>s_interval[1] and first_interval[0]>i_interval[1] and \
					second_interval[0]>f_interval[1] and second_interval[0]>s_interval[1] and second_interval[0]>i_interval[1] and \
					inter_interval[0]>f_interval[1] and inter_interval[0]>s_interval[1] and inter_interval[0]>i_interval[1] :

					interfere = False

				elif first_interval[1]<f_interval[0] and first_interval[1]<s_interval[0] and first_interval[1]<i_interval[0] and \
					second_interval[1]<f_interval[0] and second_interval[1]<s_interval[0] and second_interval[1]<i_interval[0] and \
					inter_interval[1]<f_interval[0] and inter_interval[1]<s_interval[0] and inter_interval[1]<i_interval[0] :


					interfere = False

				else: #interfere

					interfere = True	

			if not interfere: # collect the id and size of the group that can be inserted
				if insert_id == -1:
					insert_id = i
					insert_group_size = l

				elif l < insert_group_size:
					insert_id = i
					insert_group_size = l

		if insert_id == -1: # did not find an appropriate group, start a new group
			ci = {}
			ci["first_interval"] = first_interval
			ci["second_interval"] = second_interval
			ci["inter_interval"] = inter_interval
			ci["msg"] = msg_str
			group = []
			group.append(ci)
			group_set.append(group)

			#file_name = "replay/group_"+str(group_num)+".log"
			#group_fhandler = open(file_name, "w")
			#group_fhandler.write(msg_str)
			#group_fhandler.close()

			group_num = group_num + 1

			#print "start new group, group_num:", group_num
			#print ci
			#print msg_str

		else: # insert to an old group by "insert_id"
			ci = {}
			ci["first_interval"] = first_interval
			ci["second_interval"] = second_interval
			ci["inter_interval"] = inter_interval
			ci["msg"] = msg_str
			group_set[insert_id].append(ci)

			#file_name = "replay/group_"+str(insert_id)+".log"
			#group_fhandler = open(file_name, "w+")
			#group_fhandler.write(msg_str)
			#group_fhandler.close()

			#print " insert to an old group", i, "group num:", group_num
			#print ci
			#print msg_str

	# write to the group with sorted order
	for i in range(group_num):
		origin_list = group_set[i]
		l1 = len(origin_list)
		assert l1 >= 1

		sorted_list = []
		
		for j in range(l1):
			if j == 0:
				sorted_list.append(origin_list[j])
			elif j == 1:
				if origin_list[j]["first_interval"][0] < sorted_list[0]["first_interval"][0]:
					sorted_list.insert(0, origin_list[j])
				else:
					sorted_list.append(origin_list[j])
			else:
				l2 = len(sorted_list)
				if origin_list[j]["first_interval"][0] < sorted_list[0]["first_interval"][0]:
					sorted_list.insert(0, origin_list[j])
				elif origin_list[j]["first_interval"][0] > sorted_list[l2-1]["first_interval"][0]:
					sorted_list.append(origin_list[j])
				else:
					k = 0
					while k+1 < l2: 
						if origin_list[j]["first_interval"][0] > sorted_list[k]["first_interval"][0] and \
						origin_list[j]["first_interval"][0] < sorted_list[k+1]["first_interval"][0]:

							sorted_list.insert(k+1, origin_list[j])

						k = k + 1

		file_name = "replay/group_"+str(i)+".log"
		group_fhandler = open(file_name, "w")				
		for j in range(l1):
			group_fhandler.write(sorted_list[j]["msg"])
		group_fhandler.close()





# check whether we use read/write instuctions or 
# lock/unlock statements to control the thread scheduling.
def find_identifier():

	#file_result_handler = open("replay/file_result.log", "w")
	for re in result_list:
		#print "re", re
		if re["first_cs"] != None and re["second_cs"] != None and  re["inter_cs"] != None:
			if re["first_cs"]["l_ev"] ==  re["second_cs"]["l_ev"] and re["first_cs"]["l_ev"] ==  re["inter_cs"]["l_ev"]:

				msg_str = str(1)+"["+re["first_cs"]["tid"]+"]"+re["first_cs"]["l_cv"]+","+re["first_cs"]["u_cv"]+"; [" \
				+re["second_cs"]["tid"]+"]"+re["second_cs"]["l_cv"]+","+re["second_cs"]["u_cv"]+"; [" \
				+re["inter_cs"]["tid"]+"]"+re["inter_cs"]["l_cv"]+","+re["inter_cs"]["u_cv"]+"\n"

				first_interval = []
				first_interval.append(re["first_cs"]["stime"])
				first_interval.append(re["first_cs"]["ftime"])
				second_interval = []
				second_interval.append(re["second_cs"]["stime"])
				second_interval.append(re["second_cs"]["ftime"])
				inter_interval = []
				inter_interval.append(re["inter_cs"]["stime"])
				inter_interval.append(re["inter_cs"]["ftime"])

				grouper(first_interval, second_interval, inter_interval, msg_str)
			
			else: # to be refined 
				msg_str = str(0)+"["+re["first"]["tid"]+"]"+re["first"]["ip"]+","+re["first"]["addr"]+"; [" \
				+re["second"]["tid"]+"]"+re["second"]["ip"]+","+re["second"]["addr"]+"; [" \
				+re["inter"]["tid"]+"]"+re["inter"]["ip"]+","+re["inter"]["addr"]+"\n"

				first_interval = []
				first_interval.append(re["first"]["time"])
				first_interval.append(re["first"]["time"])
				second_interval = []
				second_interval.append(re["second"]["time"])
				second_interval.append(re["second"]["time"])
				inter_interval = []
				inter_interval.append(re["inter"]["time"])
				inter_interval.append(re["inter"]["time"])
				
				grouper(first_interval, second_interval, inter_interval, msg_str)

		else:
			msg_str = str(0)+"["+re["first"]["tid"]+"]"+re["first"]["ip"]+","+re["first"]["addr"]+"; [" \
			+re["second"]["tid"]+"]"+re["second"]["ip"]+","+re["second"]["addr"]+"; [" \
			+re["inter"]["tid"]+"]"+re["inter"]["ip"]+","+re["inter"]["addr"]+"\n"

			first_interval = []
			first_interval.append(re["first"]["time"])
			first_interval.append(re["first"]["time"])
			second_interval = []
			second_interval.append(re["second"]["time"])
			second_interval.append(re["second"]["time"])
			inter_interval = []
			inter_interval.append(re["inter"]["time"])
			inter_interval.append(re["inter"]["time"])
			
			grouper(first_interval, second_interval, inter_interval, msg_str)
		
		#print msg_str
		#file_result_handler.write(msg_str)	
		
	#file_result_handler.close()



def show_statics():
	print "static result=========================================="
	print "CI num:", len(ci_list)
	print "result num:", len(result_list)
	print "group_num:", group_num
	print "visited addresses num:", len(trace_mem)

	for addr in trace_mem:
		tid_list = trace_mem[addr]["tid_list"]
		print "addr: 0x",addr
		for i in range(len(tid_list)):
			print "\ttid",tid_list[i],":", len(trace_mem[addr][str(tid_list[i])]["trace_list"]),"access,", "optag:",trace_mem[addr][str(tid_list[i])]["optag"]
		



load_mem_traces()

#tool.print_mem_trace(trace_mem)

load_lock_traces()

load_sync_traces()

find_thread_paris_for_same_addr()

#tool.print_ci_list(ci_list)

#tool.print_cs_list(cs_list)

prune()

#tool.print_result_list(result_list)

find_identifier()

show_statics()
	