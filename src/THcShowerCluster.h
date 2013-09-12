#ifndef ROOT_THcShowerCluster
#define ROOT_THcShowerCluster

//HMS calorimeter hit cluster, version 3.

#include "THcShowerHit.h"

//HMS calorimeter hit cluster
//
class THcShowerCluster : THcShowerHitList {

 public:

  THcShowerCluster() {
    //    cout << "Dummy THcShowerCluster object created" << endl;
  }

  ~THcShowerCluster() {
    for (THcShowerHitIt i = THcShowerHitList::begin();
    	 i != THcShowerHitList::end(); i++) {
      delete *i;
      *i = 0;
    }
  }

  // Add a hit to the cluster hit list
  //
  void grow(THcShowerHit* hit) {
    THcShowerHitList::push_back(hit);
  }

  //Pointer to the hit #i in the cluster hit list
  //
  THcShowerHit* ClusteredHit(UInt_t i) {
    return * (THcShowerHitList::begin()+i);
  }

  //Print out a hit in the cluster
  //
  void showHit(UInt_t num) {
    (*(THcShowerHitList::begin()+num))->show();
  }

  // X coordinate of cluster's center of gravity,
  // calculated as a weighted by hit energies average.
  // Put X out of the calorimeter (-75 cm), if there is no energy deposition
  // in the cluster.
  //
  Double_t clX() {
    Double_t x_sum=0.;
    Double_t Etot=0.;
    for (THcShowerHitIt it=THcShowerHitList::begin();
	 it!=THcShowerHitList::end(); it++) {
      x_sum += (*it)->hitX() * (*it)->hitE();
      Etot += (*it)->hitE();
    }
    //    cout << "x_sum=" << x_sum << "  Etot=" << Etot << endl;
    return (Etot != 0. ? x_sum/Etot : -75.);
  }

  // Z coordinate of cluster's center of gravity,
  // calculated as a weighted by hit energies average.
  // Put Z out of the calorimeter (0 cm), if there is no energy deposition
  // in the cluster.
  //
  Double_t clZ() {
    Double_t z_sum=0.;
    Double_t Etot=0.;
    for (THcShowerHitIt it=THcShowerHitList::begin();
	 it!=THcShowerHitList::end(); it++) {
      z_sum += (*it)->hitZ() * (*it)->hitE();
      Etot += (*it)->hitE();
    }
    //    cout << "z_sum=" << z_sum << "  Etot=" << Etot << endl;
    return (Etot != 0. ? z_sum/Etot : 0.);
  }

  //Energy depostion in a cluster
  //
  Double_t clE() {
    //    cout << "In ECl:" << endl;
    Double_t Etot=0.;
    for (THcShowerHitIt it=THcShowerHitList::begin();
	 it!=THcShowerHitList::end(); it++) {
      Etot += (*it)->hitE();
    }
    return Etot;
  }

  //Energy deposition in the Preshower (1st layer) for a cluster
  //
  Double_t clEpr() {
    Double_t Epr=0.;
    for (THcShowerHitIt it=THcShowerHitList::begin();
	 it!=THcShowerHitList::end(); it++) {
      if ((*it)->hitColumn() == 0) Epr += (*it)->hitE();
    }
    return Epr;
  }

  //Cluster size (number of hits in the cluster).
  //
  UInt_t clSize() {
    return THcShowerHitList::size();
  }

};

//-----------------------------------------------------------------------------

//Alias for container of clusters and for its iterator
//
typedef vector<THcShowerCluster*> THcShClusterList;
typedef THcShClusterList::iterator THcShClusterIt;

//List of clusters
//
class THcShowerClusterList : private THcShClusterList {

 public:

  THcShowerClusterList() {
    //    cout << "Dummy THcShowerClusterList object created" << endl;
  }

  ~THcShowerClusterList() {
    for (THcShClusterIt i = THcShClusterList::begin();
    	 i != THcShClusterList::end(); i++) {
      delete *i;
      *i = 0;
    }
  }

  //Put a cluster in the cluster list
  //
  void grow(THcShowerCluster* cluster) {
    THcShClusterList::push_back(cluster);
  }

  //Pointer to the cluster #i in the cluster list
  //
  THcShowerCluster* ListedCluster(UInt_t i) {
    return *(THcShClusterList::begin()+i);
  }

  //Cluster list size.
  //
  UInt_t NbClusters() {
    return THcShClusterList::size();
  }

//_____________________________________________________________________________

void ClusterHits(THcShowerHitList HitList) {

// Cluster hits from the HitList. The resultant clusters of hits are saved
//in the ClusterList.

  while (HitList.size() != 0) {

    THcShowerCluster* cluster = new THcShowerCluster;

    (*cluster).grow(*(HitList.end()-1)); //move the last hit from the hit list
    HitList.erase(HitList.end()-1);      //into the 1st cluster
    bool clustered = true;

    while (clustered) {                   //while a hit is clustered

      clustered = false;

      for (THcShowerHitIt i=HitList.begin(); i!=HitList.end(); i++) {

	for (UInt_t k=0; k!=(*cluster).clSize(); k++) {

	  if ((**i).isNeighbour((*cluster).ClusteredHit(k))) {

	    (*cluster).grow(*i);          //If the hit #i is neighbouring a hit
	    HitList.erase(i);             //in the cluster, then move it
	                                  //into the cluster.
	    clustered = true;
	  }

	  if (clustered) break;
	}                                 //k

	if (clustered) break;
      }                                   //i

    }                                     //while clustered

    grow(cluster);                        //Put the cluster in the cluster list

  }                                       //While hit_list not exhausted

}

};

#endif