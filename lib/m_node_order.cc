#include "u_node_order.h"
#include "u_opt.h"
#include "globals.h"

void NODE_ORDER::reinit(unsigned total_nodes)
{ untested();
	if(_order){ untested();
		incomplete();
	}else{
	}

	if(!OPT::order){ itested();
		// default order is overriden in default_plugins.
		assert(order_dispatcher["default"]);
		ORDERING const* od=prechecked_cast<ORDERING*>(order_dispatcher["default"]);
		assert(od);
		OPT::order = od;
	}else{ untested();
	}

	_order = OPT::order->clone();
	assert(_order);
	::status.order.reset().start();
	_order->init(total_nodes, *this);
	::status.order.stop();
}
