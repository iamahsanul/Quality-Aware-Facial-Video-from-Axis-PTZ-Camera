/*!
 *\class bgState
 *\brief class for background subtraction module
 * background subtraction use a mixture of gaussian algorythm
 * this state is active when camera is fixed
 * this module is use to find and initialise a target to track
 */


 /*!
 *Updated by Mohammad Ahsanul Haque (iamahsanul@gmail.com)
 *Aalborg University, Denmark
 */

 /*!Foundation reference:
 *\author Fayel <loic.fayel@ecole.ensicaen.fr>
 *\author Cochet <stephanie.cochet@ecole.ensicaen.fr>
 */

#ifndef __BGSTATE_H__
#define __BGSTATE_H__

#include "cvblob.h"
#include "state.h"


#define NB_BLOB_ATTENTE 2

#define PARAM_LC 128
#define PARAM_N1C 15
#define PARAM_N2C 25
#define PARAM_LCC 64
#define PARAM_N1CC 25
#define PARAM_N2CC 40
#define PARAM_IOWH true
#define PARAM_PM 1
#define PARAM_ALPHA1 0.1f
#define PARAM_ALPHA2 0.001f
#define PARAM_ALPHA3 0.1f
#define PARAM_DELTA 2
#define PARAM_T 0.9f
#define PARAM_MINAREA 15.f

#define BLOB_TAILLE_MIN 1500
#define BLOB_TAILLE_MAX 60000

#define HAUT_FENETRE 0
#define BAS_FENETRE 240
#define GAUCHE_FENETRE 0
#define DROITE_FENETRE 320
#define PAS_FENETRE 5

#define TARGET_FIND 1
#define NO_TARGET 0

using namespace cvb;

class bgState : public state
{
	private :
		int			cpt;			/*!< compt blob*/
		unsigned int		num_last_blob;		/*!< nulber of last blob*/
		CvFGDStatModelParams*	param_background;	/*!< BGsubtraction parameters*/
		CvBGStatModel*		bg_model;		/*!< probalistic BG model*/
		IplImage*		labelImg;		/*!< IplImage of blob label*/
		CvTracks		tracks;			/*!< blob tracks*/
		CvBlobs			blobs;			/*!< list of blobs*/
		CvRect			target;			/*!< Rectangle containing the current target*/
		std::map<CvLabel,cvb::CvBlob*>::iterator idxBlobs;/*!< iterator on list of blobs*/
	public	:
		/*!
		 *\brief constructor
		 * build a new instance of bgSate class without initialised \c bg_model and \c labelImg
		 */
		bgState();
		/*!
		 *\brief constructor
		 * build a new instance of bgSate 
		 */
		bgState(IplImage* tmp_frame);
		/*!
		 *\brief initialise \c bg_model and \c labelImg with frame size and depth
		 *\param IplImage* : frame to be processing
		 */
		void init(IplImage* tmp_frame);
		/*!
		 *\brief search for a target in the frame
		 *\param IplImage* : frame to be processing
		 *\return 1 if a target is found in the frame
		 * 0 otherwise
		 */
		int process(IplImage* tmp_frame);
		/*!
		 *\brief return the current target
		 *\return current target in a CvRect* structur
		 */
		CvRect* getTarget();
		/*!
		 *\brief destructor
		 * erase an instance of bgSub 
		 */
		~bgState();
};

#endif
