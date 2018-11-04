#include "m_node_order.h"
#include "u_node_order.h"
#include "u_opt.h"
#include "globals.h"

void NODE_ORDER::reinit(unsigned total_nodes)
{ untested();
	if(_order){ untested();
		incomplete();
	}else{ untested();
	}

	if(!OPT::order){ untested();
		assert(order_dispatcher["default"]);
		ORDERING const* od=prechecked_cast<ORDERING*>(order_dispatcher["default"]);
		assert(od);
		OPT::order = od;
	}else{ untested();
	}

	_order = OPT::order->clone();
	assert(_order);
	_order->init(total_nodes, *this);
}
