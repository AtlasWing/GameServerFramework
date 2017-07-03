#ifndef _CONFIG_DATA_WRAPPER_H_
#define _CONFIG_DATA_WRAPPER_H_
#include <string>



using std::string;

/**
 * 常用的一写多读的配置对象的封装
 */
template<typename T>
class ConfigDataWrapper {
private:
	T inst0_;
	T inst1_;
    bool query_using_inst0_;
public:
    ConfigDataWrapper() :
    	query_using_inst0_(true) {
	}

    virtual ~ConfigDataWrapper() {
    }

    //获取查询的实例
    virtual T& getQueryData() {
    	if(query_using_inst0_) {
    		return inst0_;
    	}
    	else {
    		return inst1_;
    	}
    }

    //获取更新的实例
    virtual T& getUpdateData() {
    	if(query_using_inst0_) {
    		return inst1_;
    	}
    	else {
    		return inst0_;
    	}
    }

    virtual void swap() {
    	query_using_inst0_ = !query_using_inst0_;
    }
};

#endif
