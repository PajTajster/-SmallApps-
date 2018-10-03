using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Data.Entity;
using UsersData.Models;

namespace UsersData.DAL
{
    public class DB : DbContext
    {
        public DB() : base("gr2")
        {

        }
        
        public DbSet<User> UserDataBase { get; set; }
    }
}