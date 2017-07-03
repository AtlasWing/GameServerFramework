#ifndef _CONFIG_DATA_WRAPPER_H_
#define _CONFIG_DATA_WRAPPER_H_
#include <string>



using std::string;

/**
 * ���õ�һд��������ö���ķ�װ
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

    //��ȡ��ѯ��ʵ��
    virtual T& getQueryData() {
    	if(query_using_inst0_) {
    		return inst0_;
    	}
    	else {
    		return inst1_;
    	}
    }

    //��ȡ���µ�ʵ��
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
