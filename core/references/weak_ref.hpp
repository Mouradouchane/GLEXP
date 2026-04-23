
// note: weak_ref/pointer have no ownership over memory
// note: this help solve circular dependency problem when --> A refer to B and B to A !! causing memory leak !
template<typename type> class weak_ref {

	// note: weak_ref points to object but no counted -> no counter update 


}; // class weak_ref end
