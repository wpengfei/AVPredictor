import struct
import tool

mem_trace = {} # structured memory traces
ci_list = [] # list of candidate cis
cs_list = [] # list of critical sections
sync_list = [] #list of sync primitives
result_list = []

group_num = 0
group_set = []

DISTANCE = 100000 #threshold to decide whether the "inter" is related to a access pair (First, Second)

path_sync = "trace_sync.log"
path_lock = "trace_lock.log"
path_mem = "trace_mem.log"

READ = 1
WRITE = 0

LOCK = 1
UNLOCK = 0

BARRIER = 1
CONDWAIT = 2
CONDTIMEWAIT = 3
SLEEP = 4


def load_sync_traces(path_sync):
	file_sync_handler = open(path_sync, "rb")
	each_line = file_sync_handler.read(16)

	while each_line:	
		tid, time, ty = struct.unpack("@2IQ",each_line)
		#print "[load_sync]tid:",tid,"time:",hex(time),"ty:",ty
		sync = {}
		sync["tid"] = tid
		sync["time"] = time
		sync["type"] = ty 
		sync_list.append(sync)

		each_line = file_sync_handler.read(16)

	file_sync_handler.close()

def load_lock_traces(path_lock):
	file_lock_handler = open(path_lock, "rb")
	each_line = file_lock_handler.read(24)

	while each_line:

		tid, time, callsite_v, entry_v, op = struct.unpack("@IQ3I",each_line)
		#print "[load_lock]tid:",tid,"time:",hex(time),"callsite_v:",hex(callsite_v),"entry_v",hex(entry_v),"op",op

		if op == LOCK: #create a critical section 
			cs = {} # three-tuple (tid, call_lock_v, call_unlock_v) can uniquely determine a critical section 
			cs["tid"] = tid
			cs["stime"] = time #start time 
			cs["ftime"] = 0 # finish time
			cs["l_cv"] = callsite_v # lock_callsite_value
			cs["l_ev"] = entry_v # lock_entry_value
			cs["u_cv"] = 0 # unlock_callsite_value
			cs["u_ev"] = 0 # unlock_entry_value
			cs_list.append(cs)
		else: #complete the critical section
			for it in cs_list:
				if it["tid"] == tid and it["l_ev"] == entry_v and it["ftime"] == 0:
					it["ftime"] = time
					it["u_cv"] = callsite_v
					it["u_ev"] = entry_v

		each_line = file_lock_handler.read(24)

	file_lock_handler.close()


'''
mem_trace = {"addr1": addr_dict1, "addr2":addr_dict2}

addr_dict = {"thread_num":2, "tid_list":tid_list, "tid1":tid_dict1, "tid2": tid_dict2}

tid_list = [tid1,tid2]

tid_dict = {"optag": 2, "trace_list": tlist}    # optag: 0,R; 1,W; 2,R&W

tlist = [trace1, trace2]

'''
def load_mem_traces(path_mem):
	file_mem_handler = open(path_mem, "rb")
	each_line = file_mem_handler.read(24)
	while each_line:
		
		ip,addr,time,tid,op = struct.unpack("@2IQ2I", each_line)
		#print "[load_mem]ip",hex(ip),"addr:",hex(addr),"time:",hex(time),"tid:",tid,"op",op
		#ip = str(hex(ip))
		#addr = str(hex(addr))
		#tid = str(tid)
		trace = {}
		trace["tid"] = tid	
		trace["time"] = time
		trace["ip"] = ip
		trace["addr"] = addr
		trace["op"] = op
		#trace["rtn"] = rtn

		if  mem_trace.has_key(addr):
			if mem_trace[addr].has_key(tid):
				optag = mem_trace[addr][tid]["optag"]
				if  op == READ and optag == 1:
					optag = 2;
					mem_trace[addr][tid]["optag"] = optag
				elif op == WRITE and optag == 0:
					optag = 2;
					mem_trace[addr][tid]["optag"] = optag

				mem_trace[addr][tid]["trace_list"].append(trace)

			else:
				tlist = []
				tlist.append(trace)
				tid_dict = {}
				if op == READ:
					tid_dict["optag"] = 0
				else:
					tid_dict["optag"] = 1
				tid_dict["trace_list"] = tlist

				mem_trace[addr]["thread_num"] = mem_trace[addr]["thread_num"] + 1
				mem_trace[addr]["tid_list"].append(tid)
				mem_trace[addr][tid] = tid_dict

		else:
			tlist = []
			tlist.append(trace)

			tid_dict = {}
			if op == READ:
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
			
			mem_trace[addr] = addr_dict

		each_line = file_mem_handler.read(24)

	file_mem_handler.close()



def find_write_trace(tlist,time1,time2):
	assert time1 < time2
	ret = []
	for it in tlist:
		if it["op"] == READ:
			continue
		elif it["time"] > time1 and it["time"] < time2:
			continue
		#elif it["time"] - time2 > DISTANCE or time1 - it["time"] > DISTANCE:
			#continue
		else:
			ret.append(it)

	return ret

def find_read_trace(tlist,time1,time2):
	assert time1 < time2
	ret = []
	for it in tlist:
		if it["op"] == WRITE:
			continue
		elif it["time"] > time1 and it["time"] < time2:
			continue
		#elif it["time"] - time2 > DISTANCE or time1 - it["time"] > DISTANCE:
			#continue
		else:
			ret.append(it)

	return ret



def find_ci_from_thread_pair(tid_dict0, tid_dict1):
	# a "tid_dict" contains the memory traces that accessing the same addr from the same thread
	if tid_dict0["optag"] == 0 and tid_dict1["optag"] == 0:
		return None
	elif tid_dict0["optag"] == 1 and tid_dict1["optag"] == 1:
		return None

	else: 
		tlist0 = tid_dict0["trace_list"]
		tlist1 = tid_dict1["trace_list"]

		l0 = len(tlist0)
		l1 = len(tlist1)

		if l0 + l1 <= 2: # at least three accesses to for a CI
			return None

		if l0 >= 2:
			for i in range(l0-1):
				# first and second in a CI must not be interleaved by an access from the same thread
				# aka. first and second must be next to each other in the list
				# besides, the occurrences should not beyond a DISTANCE
				
				#if tlist0[i+1]["time"] - tlist0[i]["time"] > DISTANCE:
				#	continue

				if tlist0[i]["op"] == READ:
					if tlist0[i+1]["op"] == READ:
						ret_list = find_write_trace(tlist1,tlist0[i]["time"],tlist0[i+1]["time"])
						for it in ret_list:
							ci = {}
							ci["first"] = tlist0[i]
							ci["second"] = tlist0[i+1]
							ci["inter"] = it
							ci_list.append(ci)
					elif tlist0[i+1]["op"] == WRITE:
						ret_list = find_write_trace(tlist1,tlist0[i]["time"],tlist0[i+1]["time"])
						for it in ret_list:
							ci = {}
							ci["first"] = tlist0[i]
							ci["second"] = tlist0[i+1]
							ci["inter"] = it
							ci_list.append(ci)

				elif tlist0[i]["op"] == WRITE:
					if tlist0[i+1]["op"] == READ:
						ret_list = find_write_trace(tlist1,tlist0[i]["time"],tlist0[i+1]["time"])
						for it in ret_list:
							ci = {}
							ci["first"] = tlist0[i]
							ci["second"] = tlist0[i+1]
							ci["inter"] = it
							ci_list.append(ci)
				
					elif tlist0[i+1]["op"] == WRITE:
						ret_list = find_read_trace(tlist1,tlist0[i]["time"],tlist0[i+1]["time"])
						for it in ret_list:
							ci = {}
							ci["first"] = tlist0[i]
							ci["second"] = tlist0[i+1]
							ci["inter"] = it
							ci_list.append(ci)

		if l1 >= 2:
			for i in range(l1-1): # switch tlist0 and tlist 1

				#if tlist1[i+1]["time"] - tlist1[i]["time"] > DISTANCE: #abandon the unlikely cases
				#	continue
			
				if tlist1[i]["op"] == READ:
					if tlist1[i+1]["op"] == READ:
						ret_list = find_write_trace(tlist0,tlist1[i]["time"],tlist1[i+1]["time"])
						for it in ret_list:
							ci = {}
							ci["first"] = tlist1[i]
							ci["second"] = tlist1[i+1]
							ci["inter"] = it
							ci_list.append(ci)

					elif tlist1[i+1]["op"] == WRITE:
						ret_list = find_write_trace(tlist0,tlist1[i]["time"],tlist1[i+1]["time"])
						for it in ret_list:
							ci = {}
							ci["first"] = tlist1[i]
							ci["second"] = tlist1[i+1]
							ci["inter"] = it
							ci_list.append(ci)

				elif tlist1[i]["op"] == WRITE:
					if tlist1[i+1]["op"] == READ:
						ret_list = find_write_trace(tlist0,tlist1[i]["time"],tlist1[i+1]["time"])
						for it in ret_list:
							ci = {}
							ci["first"] = tlist1[i]
							ci["second"] = tlist1[i+1]
							ci["inter"] = it
							ci_list.append(ci)
					elif tlist1[i+1]["op"] == WRITE:
						ret_list = find_read_trace(tlist0,tlist1[i]["time"],tlist1[i+1]["time"])
						for it in ret_list:
							ci = {}
							ci["first"] = tlist1[i]
							ci["second"] = tlist1[i+1]
							ci["inter"] = it
							ci_list.append(ci)



def predictor():
	# start by finding thread paris for the same addr
	for addr in mem_trace:
		if mem_trace[addr]["thread_num"] < 2:
			#print "thread num < 2"
			continue
		else:
			tid_list = mem_trace[addr]["tid_list"]
			l = len(tid_list)
			assert l >= 2
			if l == 2: # for most of the cases, only two threads involved
				tid_dict0 = mem_trace[addr][tid_list[0]]
				tid_dict1 = mem_trace[addr][tid_list[1]]

				find_ci_from_thread_pair(tid_dict0, tid_dict1) #directed pairs
						
			else:
				for i in range(l):
					tid_dict0 = mem_trace[addr][tid_list[i]]
					for j in range(i+1,l):
						tid_dict1 = mem_trace[addr][tid_list[j]]
						find_ci_from_thread_pair(tid_dict0, tid_dict1) #directed pairs

def pruner():
	# remove the infesible cases from the ci_list
	for ci in ci_list:
		first = ci["first"]
		second = ci["second"]
		inter = ci["inter"]

		assert first["tid"] == second["tid"]

		# remove the cases that firsr and Second are next to each other, mainly in the main thread
		#if first["time"] + 1 == second["time"]: 
		#	continue

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
		#group_fhandler = open("work_dir/replay/group_0.log", "w")
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
					break	

			if not interfere: # collect the id and size of the group that can be inserted
				if insert_id == -1:
					insert_id = i
					insert_group_size = l

				else:
					if l < insert_group_size: # find the smallest one
						insert_id = i
						insert_group_size = l
				

		if insert_id == -1 : # did not find an appropriate group, start a new group
			ci = {}
			ci["first_interval"] = first_interval
			ci["second_interval"] = second_interval
			ci["inter_interval"] = inter_interval
			ci["msg"] = msg_str
			group = []
			group.append(ci)
			group_set.append(group)

			#file_name = "work_dir/replay/group_"+str(group_num)+".log"
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

			#file_name = "work_dir/replay/group_"+str(insert_id)+".log"
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
				#print "--1--",sorted_list
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
							break

						k = k + 1
				#print "--2--",sorted_list

		#print "l1 =",l1,"sorted_list =", len(sorted_list)
		#print "origin",origin_list
		#print "sorted",sorted_list
		assert l1 == len(sorted_list)

		file_name = "groupset/group_"+str(i)+".log"
		group_fhandler = open(file_name, "w")				
		for j in range(len(sorted_list)):
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

				msg_str = str(1)+"["+str(re["first_cs"]["tid"])+"]"+str(re["first_cs"]["l_cv"])+","+str(re["first_cs"]["u_cv"])+"; [" \
				+str(re["second_cs"]["tid"])+"]"+str(re["second_cs"]["l_cv"])+","+str(re["second_cs"]["u_cv"])+"; [" \
				+str(re["inter_cs"]["tid"])+"]"+str(re["inter_cs"]["l_cv"])+","+str(re["inter_cs"]["u_cv"])+"\n"

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
				msg_str = str(0)+"["+str(re["first"]["tid"])+"]"+str(re["first"]["ip"])+","+str(re["first"]["addr"])+"; [" \
				+str(re["second"]["tid"])+"]"+str(re["second"]["ip"])+","+str(re["second"]["addr"])+"; [" \
				+str(re["inter"]["tid"])+"]"+str(re["inter"]["ip"])+","+str(re["inter"]["addr"])+"\n"

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
			msg_str = str(0)+"["+str(re["first"]["tid"])+"]"+str(re["first"]["ip"])+","+str(re["first"]["addr"])+"; [" \
			+str(re["second"]["tid"])+"]"+str(re["second"]["ip"])+","+str(re["second"]["addr"])+"; [" \
			+str(re["inter"]["tid"])+"]"+str(re["inter"]["ip"])+","+str(re["inter"]["addr"])+"\n"

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
		
		#print "msg_str", msg_str
		#file_result_handler.write(msg_str)	
		
	#file_result_handler.close()




			



load_mem_traces(path_mem)

load_lock_traces(path_lock)

load_sync_traces(path_sync)

#tool.print_mem_trace(mem_trace)

#tool.print_cs_list(cs_list)

#tool.print_sync_list(sync_list)



predictor()

#tool.print_ci_list(ci_list)


pruner()

#tool.print_result_list(result_list)

find_identifier()


tool.show_statistics(mem_trace,cs_list,sync_list,ci_list,result_list,group_num)
	