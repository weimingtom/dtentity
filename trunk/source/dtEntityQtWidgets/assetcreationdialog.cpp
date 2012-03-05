/*
* dtEntity Game and Simulation Engine
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* Martin Scheffler
*/

#include <dtEntityQtWidgets/assetcreationdialog.h>
#include <QtGui/QtGui>
#include "ui_assetcreationdialog.h"

namespace dtEntityQtWidgets
{
   ////////////////////////////////////////////////////////////////////////////////
   AssetCreationDialog::AssetCreationDialog(const QStringList& datapaths, const QString& filename, const QString& extension)
      : mExtension(extension)
   { 
      Ui_AssetCreationDialog dialog;
      dialog.setupUi(this);
      mDataPathChooser = dialog.mDataPathChooser;
      mFileName = dialog.mFileName;
      mFileName->setText(filename);
      QString path;
      foreach(path, datapaths)
      {
         mDataPathChooser->addItem(path);
      }
      dialog.mExtension->setText(extension);
   }

   ////////////////////////////////////////////////////////////////////////////////
   AssetCreationDialog::~AssetCreationDialog()
   { 
   }


   ////////////////////////////////////////////////////////////////////////////////
   QString AssetCreationDialog::GetDataPath() const
   {      
      return mDataPathChooser->currentText();
   }

   ////////////////////////////////////////////////////////////////////////////////
   QString AssetCreationDialog::GetMapPath() const
   {
      return QString("%2%3").arg(mFileName->text()).arg(mExtension);
   }
}