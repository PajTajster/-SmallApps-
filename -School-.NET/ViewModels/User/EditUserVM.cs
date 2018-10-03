using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using UsersData.Models;

namespace UsersData.ViewModels.User
{
    public class EditUserVM : UserVM
    {
        EditUserVM()
        {

        }
        public EditUserVM(Models.User user) : base(user)
        {

        }
    }
}