/*!
 brief methods for background subtraction module
*/


 /*!
 *Updated by Mohammad Ahsanul Haque (iamahsanul@gmail.com)
 *Aalborg University, Denmark
 */

 /*!Foundation reference:
 *\author Fayel <loic.fayel@ecole.ensicaen.fr>
 *\author Cochet <stephanie.cochet@ecole.ensicaen.fr>
 */
 
#include "../include/bgState.h"


bgState::bgState()
{
	id=BG_STATE;
	name="BGsub";
	cpt=NB_BLOB_ATTENTE;
	//background subtraction parameters
	param_background= new CvFGDStatModelParams();
	param_background->Lc=PARAM_LC;
	param_background->N1c=PARAM_N1C;
	param_background->N2c=PARAM_N2C;
	param_background->Lcc=PARAM_LCC;
	param_background->N1cc=PARAM_N1CC;
	param_background->N2cc=PARAM_N2CC;
	param_background->is_obj_without_holes=PARAM_IOWH;
	param_background->perform_morphing=PARAM_PM;
	param_background->alpha1=PARAM_ALPHA1;
	param_background->alpha2=PARAM_ALPHA2;
	param_background->alpha3=PARAM_ALPHA3;
	param_background->delta=PARAM_DELTA;
	param_background->T=PARAM_T;
	param_background->minArea=PARAM_MINAREA;
}

bgState::bgState(IplImage* tmp_frame)
{
	cpt=NB_BLOB_ATTENTE;
	id=BG_STATE;
	name="BGsub";
	//background subtraction parameters
	param_background= new CvFGDStatModelParams();
	param_background->Lc=PARAM_LC;
	param_background->N1c=PARAM_N1C;
	param_background->N2c=PARAM_N2C;
	param_background->Lcc=PARAM_LCC;
	param_background->N1cc=PARAM_N1CC;
	param_background->N2cc=PARAM_N2CC;
	param_background->is_obj_without_holes=PARAM_IOWH;
	param_background->perform_morphing=PARAM_PM;
	param_background->alpha1=PARAM_ALPHA1;
	param_background->alpha2=PARAM_ALPHA2;
	param_background->alpha3=PARAM_ALPHA3;
	param_background->delta=PARAM_DELTA;
	param_background->T=PARAM_T;
	param_background->minArea=PARAM_MINAREA;

	bg_model = cvCreateFGDStatModel(tmp_frame, param_background);
	labelImg = cvCreateImage(cvGetSize(tmp_frame), IPL_DEPTH_LABEL, 1);
}

void bgState::init(IplImage* tmp_frame)
{
	bg_model = cvCreateFGDStatModel(tmp_frame, param_background);
	labelImg = cvCreateImage(cvGetSize(tmp_frame), IPL_DEPTH_LABEL, 1);
}

int bgState::process(IplImage* tmp_frame)
{
	//background subtraction
	cvUpdateBGStatModel(tmp_frame, bg_model);
	//blob tracking
	cvLabel(bg_model->foreground, labelImg, blobs);
	cvFilterByArea(blobs, BLOB_TAILLE_MIN, BLOB_TAILLE_MAX);
	cvUpdateTracks(blobs, tracks, 5., 10);
	cvRenderTracks(tracks,tmp_frame,tmp_frame,CV_TRACK_RENDER_ID|CV_TRACK_RENDER_BOUNDING_BOX);

	/* Traking objet detection */
	//loop on all blobs
	idxBlobs=blobs.begin();
	while(idxBlobs!=blobs.end())
	{
		//wait for NB_BLOB_ATTENTE to find interesting blobs
		if(idxBlobs->first==num_last_blob)
		{
			if(cpt!=0)
			{
				cpt=cpt-1;
			}
		}
		else
		{
			num_last_blob=idxBlobs->first;
			cpt=NB_BLOB_ATTENTE;
		}
	
		//if interesting blob and blob in the central part of the scene
		if(cpt==0) 
		{
			if(/*(idxBlobs->second->minx<GAUCHE_FENETRE+PAS_FENETRE)
				|| (idxBlobs->second->miny<BAS_FENETRE+PAS_FENETRE)
				|| (idxBlobs->second->maxx>DROITE_FENETRE-PAS_FENETRE)
				|| */(idxBlobs->second->maxy>HAUT_FENETRE-PAS_FENETRE))
			{
				cpt=NB_BLOB_ATTENTE;
			}
			else
			{
				int x=((6*idxBlobs->second->minx)+4*idxBlobs->second->maxx)/10;
				int y=((6*idxBlobs->second->miny)+4*idxBlobs->second->maxy)/10;
				int width=2*(idxBlobs->second->maxx-idxBlobs->second->minx)/10;
				int height=2*(idxBlobs->second->maxy-idxBlobs->second->miny)/10;
				target=cvRect(x,y,width,height);
				return TARGET_FIND;				
			}
		}
		idxBlobs++;
	}
	return NO_TARGET;
}

CvRect* bgState::getTarget()
{
	return &target;
}

bgState::~bgState()
{
	cvReleaseBlobs(blobs);
	cvReleaseImage(&labelImg);
	cvReleaseBGStatModel(&bg_model);
}
