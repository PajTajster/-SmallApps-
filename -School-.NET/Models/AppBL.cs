using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace UsersData.Models
{
    public class AppBL
    {
        public bool isValidUser(string username, string password)
        {
            if (username == "admin" && password == "admin")
                return true;
      
            return false;
        }
    }
}