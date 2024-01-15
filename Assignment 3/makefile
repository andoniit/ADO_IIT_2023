.PHONY: all
all: test_assign3_1 test_expr

test_assign3_1: test_assign3_1.c storage_mgr.c buffer_mgr.c buffer_mgr_stat.c dberror.c expr.c rm_serializer.c record_mgr.c 
	gcc -o test_assign3_1 test_assign3_1.c storage_mgr.c buffer_mgr.c buffer_mgr_stat.c dberror.c expr.c rm_serializer.c record_mgr.c 
test_expr: test_expr.c storage_mgr.c buffer_mgr.c buffer_mgr_stat.c dberror.c expr.c rm_serializer.c record_mgr.c 
	gcc -o test_expr test_expr.c storage_mgr.c buffer_mgr.c buffer_mgr_stat.c dberror.c expr.c rm_serializer.c record_mgr.c 

.PHONY: clean
clean2_1:	
	rm test_assign3_1
clean2_2:	
	rm test_expr 
