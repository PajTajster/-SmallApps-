﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using UsersData.Models;

namespace UsersData.ViewModels.User
{
    public class DetailsUserVM : UserVM
    {
        DetailsUserVM()
        {

        }
        public DetailsUserVM(Models.User user) : base(user)
        {

        }
    }
}